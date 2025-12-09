# Reflow Hot Plate Controller
A custom controller board that turns a cheap aluminum hot plate and heating element into a precision reflow soldering station with PID temperature control and different reflow profiles.

## Finalized Schematic:
<img width="858" height="606" alt="Screenshot 2025-12-08 at 11 57 16 AM" src="https://github.com/user-attachments/assets/25022ba3-755f-4192-a40c-60bf0360d1a7" />

## Finalized Layout:
<img width="721" height="374" alt="Screenshot 2025-12-08 at 11 56 39 AM" src="https://github.com/user-attachments/assets/5404852f-d269-4198-82a7-e92fcccc9941" />

## Finalized Render:
<img width="614" height="286" alt="Screenshot 2025-12-08 at 11 56 58 AM" src="https://github.com/user-attachments/assets/e93b87c8-d952-43af-be51-6bab674ffcf0" />

## Finalized Firmware:
A minimal firmware example demonstrating core functionality of the Reflow Hot Plate Controller is available. It initializes SPI thermocouple sensing, runs a basic PID loop to regulate temperature, and drives the MOC3021-triggered TRIAC to control the heating element. This code provides a working control loop at ~10 Hz and serves as a starting point for integrating the touchscreen UI, reflow profiles, and SD logging in later revisions.
