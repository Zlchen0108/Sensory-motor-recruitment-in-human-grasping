import json
import re
import sys
import time
import urllib.parse
import urllib.request
import zipfile
from pathlib import Path
from difflib import SequenceMatcher
from html import escape


SOURCE_DIR = Path(r"D:\BaiduSyncdisk\论文---规律论文\论文文稿\英文版本2.0\Artical\6.Artical\Reference")
OUT_DOCX = Path("nature_reference_list.docx").resolve()
OUT_JSON = Path("nature_reference_match_report.json").resolve()


MANUAL_QUERY = {
    "28": "A review on deep learning for vision-based hand detection, hand pose estimation and gesture recognition",
    "32": "ContactPose: A Dataset of Grasps with Object Contact and Hand Pose",
    "34": "PressureVision: Estimating Hand Pressure from a Single RGB Image",
    "39": "Recent Advances and Prospects in Skin-Integrated Electronics Flexible Sensing Systems",
    "40": "EgoPressure: A Dataset for Hand Pressure and Pose Estimation",
    "49": "Data-Driven Optimization of Tactile Sensor Configurations for Efficient Grasp Classification",
}

MANUAL_REFERENCE = {
    "18": "Kapandji, A. I. The Physiology of the Joints, Volume 1: Upper Limb, 6th edn (Churchill Livingstone, 2007).",
    "36": "Song, Y. R. et al. OPENTOUCH: Bringing Full-Hand Touch to Real-World Interaction. Preprint at https://doi.org/10.48550/arXiv.2512.16842 (2025).",
    "39": "Lu, X. et al. Recent Advances and Prospects in Skin-Integrated Electronics: Flexible Sensing Systems for Robotics, Human-Machine Interaction, and Health Monitoring. Adv. Mater. (2026). https://doi.org/10.1002/adma.73436",
    "49": "Guo, H., Wang, H., Li, Z., Bai, H. & Tao, L. Data-Driven Optimization of Tactile Sensor Configurations for Efficient Dexterous Manipulation. Preprint at https://doi.org/10.48550/arXiv.2409.20473 (2024).",
}


JOURNAL_ABBREV = {
    "Nature Communications": "Nat. Commun.",
    "Nature": "Nature",
    "Science": "Science",
    "Science Robotics": "Sci. Robot.",
    "Advanced Science": "Adv. Sci.",
    "IEEE Transactions on Robotics": "IEEE Trans. Robot.",
    "IEEE Robotics and Automation Letters": "IEEE Robot. Autom. Lett.",
    "IEEE Transactions on Haptics": "IEEE Trans. Haptics",
    "IEEE Transactions on Pattern Analysis and Machine Intelligence": "IEEE Trans. Pattern Anal. Mach. Intell.",
    "IEEE Transactions on Industrial Electronics": "IEEE Trans. Ind. Electron.",
    "IEEE Transactions on Biomedical Engineering": "IEEE Trans. Biomed. Eng.",
    "IEEE Access": "IEEE Access",
    "Robotics and Computer-Integrated Manufacturing": "Robot. Comput.-Integr. Manuf.",
    "Neuroscience & Biobehavioral Reviews": "Neurosci. Biobehav. Rev.",
    "The Journal of Physiology": "J. Physiol.",
    "eLife": "eLife",
    "Scientific Reports": "Sci. Rep.",
    "Sensors": "Sensors",
    "Measurement": "Measurement",
    "Computers & Graphics": "Comput. Graph.",
    "Journal of Visual Communication and Image Representation": "J. Vis. Commun. Image Represent.",
    "Trends in Cognitive Sciences": "Trends Cogn. Sci.",
}


def sort_key(path):
    m = re.match(r"^(\d+)\.", path.name)
    return (int(m.group(1)) if m else 9999, path.name.lower())


def file_id(path):
    m = re.match(r"^(\d+)\.", path.name)
    return m.group(1) if m else ""


def clean_query(path):
    stem = path.stem
    m = re.match(r"^(\d+)\.(.*)$", stem)
    num = m.group(1) if m else ""
    text = m.group(2) if m else stem
    if num in MANUAL_QUERY:
        return MANUAL_QUERY[num]
    text = text.replace("_", " ")
    text = text.replace("‐", "-").replace("‑", "-").replace("–", "-").replace("—", "-")
    text = re.sub(r"\s+", " ", text).strip()
    text = text.replace("forenhanced", "for enhanced")
    text = text.replace("tactilesystem", "tactile system")
    text = text.replace("inrobot", "in robot")
    text = text.replace("handover", "handing over")
    text = text.replace("localizedfingerpad", "localized fingerpad")
    return text


def norm(s):
    s = s.lower()
    s = re.sub(r"[^a-z0-9]+", " ", s)
    return re.sub(r"\s+", " ", s).strip()


def score_title(query, title):
    q = norm(query)
    t = norm(title)
    if not q or not t:
        return 0.0
    ratio = SequenceMatcher(None, q, t).ratio()
    containment = min(len(q), len(t)) / max(len(q), len(t)) if (q in t or t in q) else 0
    return max(ratio, containment)


def crossref_search(query):
    params = {
        "query.title": query,
        "rows": "5",
    }
    url = "https://api.crossref.org/works?" + urllib.parse.urlencode(params)
    req = urllib.request.Request(url, headers={"User-Agent": "Codex reference formatter (mailto:none@example.com)"})
    with urllib.request.urlopen(req, timeout=20) as resp:
        data = json.loads(resp.read().decode("utf-8"))
    return data.get("message", {}).get("items", [])


def first_date_year(item):
    for key in ("published-print", "published-online", "published", "issued"):
        parts = item.get(key, {}).get("date-parts")
        if parts and parts[0]:
            return str(parts[0][0])
    return ""


def authors_nature(authors):
    if not authors:
        return ""
    parsed = []
    for a in authors:
        family = a.get("family", "").strip()
        given = a.get("given", "").strip()
        if not family:
            name = (given or a.get("name", "")).strip()
            if name:
                parsed.append(name)
            continue
        initials = ""
        if given:
            tokens = re.split(r"[\s\-]+", given)
            initials = " ".join(t[0].upper() + "." for t in tokens if t)
        parsed.append((family + (", " + initials if initials else "")).strip())
    if len(parsed) > 5:
        return parsed[0] + " et al."
    if len(parsed) == 1:
        return parsed[0]
    return ", ".join(parsed[:-1]) + " & " + parsed[-1]


def journal_title(item):
    full = (item.get("container-title") or [""])[0]
    short = (item.get("short-container-title") or [""])[0]
    return JOURNAL_ABBREV.get(full, short or full)


def format_reference(item, fallback_title):
    authors = authors_nature(item.get("author", []))
    title = (item.get("title") or [fallback_title])[0].strip().rstrip(".")
    journal = journal_title(item)
    volume = item.get("volume", "")
    page = item.get("page", "")
    year = first_date_year(item)
    doi = item.get("DOI", "")

    bits = []
    if authors:
        bits.append(authors + ".")
    bits.append(title + ".")
    if journal:
        if volume and page:
            bits.append(f"{journal} {volume}, {page} ({year}).")
        elif volume:
            bits.append(f"{journal} {volume} ({year}).")
        elif year:
            bits.append(f"{journal} ({year}).")
        else:
            bits.append(journal + ".")
    elif year:
        bits.append(f"({year}).")
    if doi:
        bits.append("https://doi.org/" + doi)
    return " ".join(bits)


def fallback_reference(path, query):
    return f"{query}. [Metadata not matched; please verify against source file: {path.name}]"


def make_document_xml(paragraphs):
    pxml = []
    for text, kind in paragraphs:
        if kind == "title":
            pxml.append(
                '<w:p><w:pPr><w:pStyle w:val="Title"/></w:pPr>'
                f'<w:r><w:t>{escape(text)}</w:t></w:r></w:p>'
            )
        elif kind == "note":
            pxml.append(
                '<w:p><w:pPr><w:pStyle w:val="Normal"/></w:pPr>'
                f'<w:r><w:t>{escape(text)}</w:t></w:r></w:p>'
            )
        else:
            pxml.append(
                '<w:p><w:pPr><w:ind w:left="720" w:hanging="360"/></w:pPr>'
                f'<w:r><w:t xml:space="preserve">{escape(text)}</w:t></w:r></w:p>'
            )
    body = "".join(pxml)
    return (
        '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
        '<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">'
        f"<w:body>{body}<w:sectPr><w:pgSz w:w=\"11906\" w:h=\"16838\"/>"
        '<w:pgMar w:top="1440" w:right="1440" w:bottom="1440" w:left="1440" '
        'w:header="708" w:footer="708" w:gutter="0"/></w:sectPr></w:body></w:document>'
    )


def write_docx(paragraphs, out_path):
    content_types = (
        '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
        '<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">'
        '<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>'
        '<Default Extension="xml" ContentType="application/xml"/>'
        '<Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>'
        '<Override PartName="/word/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>'
        '</Types>'
    )
    rels = (
        '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
        '<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">'
        '<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>'
        '</Relationships>'
    )
    styles = (
        '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
        '<w:styles xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">'
        '<w:style w:type="paragraph" w:default="1" w:styleId="Normal"><w:name w:val="Normal"/>'
        '<w:rPr><w:sz w:val="22"/><w:szCs w:val="22"/></w:rPr></w:style>'
        '<w:style w:type="paragraph" w:styleId="Title"><w:name w:val="Title"/>'
        '<w:rPr><w:b/><w:sz w:val="32"/><w:szCs w:val="32"/></w:rPr></w:style>'
        '</w:styles>'
    )
    with zipfile.ZipFile(out_path, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr("[Content_Types].xml", content_types)
        z.writestr("_rels/.rels", rels)
        z.writestr("word/document.xml", make_document_xml(paragraphs))
        z.writestr("word/styles.xml", styles)


def main():
    if not SOURCE_DIR.exists():
        raise SystemExit(f"Source folder not found: {SOURCE_DIR}")

    files = sorted([p for p in SOURCE_DIR.iterdir() if p.is_file()], key=sort_key)
    seen_doi = set()
    records = []
    paragraphs = [
        ("Nature-style reference list", "title"),
        (f"Source folder: {SOURCE_DIR}", "note"),
        ("Format: Author(s). Title. Journal volume, pages (year). DOI URL.", "note"),
        ("", "note"),
    ]

    for path in files:
        num = file_id(path)
        query = clean_query(path)
        if num in MANUAL_REFERENCE:
            records.append({
                "file": path.name,
                "query": query,
                "status": "manual",
                "score": 1.0,
                "matched_title": "",
                "doi": "",
                "error": "",
                "reference": MANUAL_REFERENCE[num],
            })
            continue

        best = None
        best_score = 0.0
        error = ""
        try:
            items = crossref_search(query)
            for item in items:
                title = (item.get("title") or [""])[0]
                sc = score_title(query, title)
                if sc > best_score:
                    best = item
                    best_score = sc
            time.sleep(0.2)
        except Exception as exc:
            error = str(exc)

        if best and best_score >= 0.62:
            doi = (best.get("DOI") or "").lower()
            if doi and doi in seen_doi:
                status = "duplicate"
                ref = ""
            else:
                if doi:
                    seen_doi.add(doi)
                status = "matched" if best_score >= 0.82 else "check"
                ref = format_reference(best, query)
        else:
            status = "unmatched"
            ref = fallback_reference(path, query)

        records.append({
            "file": path.name,
            "query": query,
            "status": status,
            "score": round(best_score, 3),
            "matched_title": ((best or {}).get("title") or [""])[0] if best else "",
            "doi": (best or {}).get("DOI", "") if best else "",
            "error": error,
            "reference": ref,
        })

    display_records = [r for r in records if r["status"] != "duplicate"]
    for idx, rec in enumerate(display_records, start=1):
        paragraphs.append((f"{idx}. {rec['reference']}", "ref"))

    needs = [r for r in display_records if r["status"] in ("check", "unmatched")]
    if needs:
        paragraphs.append(("", "note"))
        paragraphs.append(("Items needing manual verification", "title"))
        for r in needs:
            paragraphs.append((f"- {r['file']} | status={r['status']} | query={r['query']} | matched={r['matched_title']}", "note"))

    write_docx(paragraphs, OUT_DOCX)
    OUT_JSON.write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"Wrote {OUT_DOCX}")
    print(f"Wrote {OUT_JSON}")
    print(f"Files: {len(files)}; references after duplicate removal: {len(display_records)}; needs check: {len(needs)}")


if __name__ == "__main__":
    main()
