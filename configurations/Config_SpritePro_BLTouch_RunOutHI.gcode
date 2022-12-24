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
C104 U1 T300           ; Set Max Hotend Temperature
M306 C15.3 A0.118 F0.131 R0.25 ; set MPC parameters for hotend
M304 P462.10 I85.47 D624.59  ; set PID parameters for BED

; Probe settings
C29 L25.00 R205.00 F25.00 B205.00 N5 T50 ; Set Mesh 25 insets, size of 5x5 and bed at 50°C
M851 X-43.5 Y-5.7      ; Set offset to nozzle for probe
C851 S480 M2           ; Set probe Z feed speed (S) in cm/s, Multiple probing (Mn>1) or disables it (M0), in manual mesh version, Z parameter can be used to set a manual Z-offset.

; filament sensor and runout
C412 M1                ; set run-out sensor active state (M0:LOW, M1:HIGH, M2:MOTION)
C412 S1 D200           ; enable filament sensor with 200mm runout

; Exttruder settings
; C562 E1              ; invert the Extruder (E1 to invert)
M92 E424.9             ; set extruder steps / mm
M603 L25.0 U30.0      ; Configure Filament Change load 410, unload 550mm
; fwretraction
M207 F2400 S0.8 Z0     ; feedrate of 40*60 retract distance 5 and z hop 0
M208 F2400 S0          ; feedrate of 40*60 de-retract extra distance 0

; customization
C810 A0 B1 C2 D3 E4    ; sets up the toolbar shortcuts A..E to funtions 0..16
;  Toolbar options for C810 A-E
;  0 AUTO_HOME
;  1 TRAMMING_WIZARD
;  2 PROBE_WIZARD
;  3 AUTO_MESH
;  4 DISABLE_STEPPERS
;  5 COOLDOWN
;  6 PREHEAT PLA
;  7 PREHEAT ABS
;  8 PREHEAT PETG
;  9 PREHEAT TPU
; 10 PREHEAT WARMUP
; 11 RUNOUT_TOGGLE
; 12 FILAMENT_MGT
; 13 HOST_SHUTDOWN
; 14 RESET_PRINTER
; 15 BRIGHTNESS_OFF
; 16 STORE_EEPROM
;
M255 S0                ; Disable LCD Timeout
M256 B127              ; Set LCD Brightness 
M413 S0                ; Disable Powerloss recovery
; C11 En Rn Gn Bn      : colorize UI elements (C11 E0 to update the screen)

M500                   ; Save all settings
;-----------------------------------------------------
G4 S1                  ; Wait a second
M300 P200              ; Beep
M117 Configuration Applied

M117 Set Z Offset from Wizard
M117 Tram Bed
M117 Create Mesh
