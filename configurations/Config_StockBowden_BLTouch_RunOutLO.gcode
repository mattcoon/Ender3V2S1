; Professional Firmware Configuration File
;=====================================================
C10                    ; Mark as a configuration file
M117 Applying configuration
;-----------------------------------------------------
; reset settings to defaults
M502                   ; reset all settings to defaults

; physical settings
C100 X0 Y0             ; Set XY Min position to 0
C101 X240 Y231 Z240    ; Set XYZ Max position
C102 X230 Y230         ; Set a bed size of 230x230
C125 X240 Y221 Z20     ; setup the park position

; Bed and Hotend specific changes
C104 U1 T275           ; Set Max Hotend Temperature
M306 C15.3 A0.118 F0.131 R0.25 ; set MPC parameters for hotend
M304 P462.10 I85.47 D624.59  ; set PID parameters for BED

; Probe settings
C29 L25.00 R205.00 F25.00 B205.00 N5 T50 ; Set Mesh 25 insets, size of 5x5 and bed at 50°C
M851 X-43.5 Y-5.7      ; Set offset to nozzle for probe
C851 S480 M2           ; Set probe Z feed speed (S) in cm/s, Multiple probing (Mn>1) or disables it (M0), in manual mesh version, Z parameter can be used to set a manual Z-offset.

; filament sensor and runout
C412 M0                ; set run-out sensor active state (M0:LOW, M1:HIGH, M2:MOTION)
C412 S1 D200           ; enable filament sensor with 200mm runout

; Exttruder settings
; C562 E1              ; invert the Extruder (E1 to invert)
M92 E93                ; set extruder steps / mm
M603 L410.00 U550.00   ; Configure Filament Change load 410, unload 550mm
; fwretraction
M207 F2400 S5 Z0       ; feedrate of 40*60 retract distance 5 and z hop 0
M208 F2400 S0          ; feedrate of 40*60 de-retract extra distance 0

; customization
C810 A0 B1 C2 D3 E4    ; sets up the toolbar shortcuts A..E to funtions 0..16
;  Toolbar options for C810 A-E
;  0 AUTO_HOME
;  1 HOME_XY
;  2 TRAMMING_WIZARD
;  3 PROBE_WIZARD
;  4 AUTO_MESH
;  5 DISABLE_STEPPERS
;  6 COOLDOWN
;  7 PREHEAT PLA
;  8 PREHEAT ABS
;  9 PREHEAT PETG
; 10 PREHEAT TPU
; 11 PREHEAT WARMUP
; 12 RUNOUT_TOGGLE
; 13 FILAMENT_MGT
; 14 HOST_SHUTDOWN
; 15 RESET_PRINTER
; 16 BRIGHTNESS_OFF
; 17 STORE_EEPROM
; 18 TOGGLE_LASER_MODE
;
M255 S0                ; Disable LCD Timeout
M256 B127              ; Set LCD Brightness 
M413 S0                ; Disable Powerloss recovery
C120 I9 T1 F1          ; iconset 9 (default), time in HMS format and fanspeed in percent
C3 F O20 H20           ; set to fan mode and laser off limit to 20, Height 20mm
; C11 En Rn Gn Bn      : colorize UI elements (C11 E0 to update the screen)

M500                   ; Save all settings
;-----------------------------------------------------
G4 S1                  ; Wait a second
M300 P200              ; Beep
M117 Configuration Applied

M117 Set Z Offset from Wizard
M117 Tram Bed
M117 Create Mesh
