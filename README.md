# Sensory-motor Recruitment in Human Grasping

This repository contains firmware code and analysis scripts for studying sensory-motor recruitment in human grasping.

## Repository Structure

| Folder | Description |
| --- | --- |
| `EID-hand-control/` | STM32/Keil firmware for the hand-control system |
| `Force correlation analysis/` | Force correlation analysis scripts and results |
| `Region correlation analysis/` | Region-level correlation analysis |
| `Hand region analysis/` | Hand-region related analysis |
| `Region importance analysis/` | Region importance analysis |
| `Selective motor recruitment analysis/` | Selective motor recruitment analysis |

## Firmware

The embedded control code targets an STM32F103C8 microcontroller and is organized as a Keil MDK/uVision project.

Main project file:

`Project.uvprojx`

## Analysis

The analysis folders contain scripts, intermediate files, and results used to investigate force, region-level correlation, and selective motor recruitment patterns during grasping.

## Notes

Build outputs, temporary files, and local Keil session files are excluded using `.gitignore`.

## Author

Zlchen0108
