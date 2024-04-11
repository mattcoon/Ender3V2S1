/**
 * Custom G-code implementation for PRO UI
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 3.1.0
 * Date: 2023/10/27
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../inc/MarlinConfigPre.h"

#if ALL(PROUI_EX, HAS_CGCODE)

#include "../MarlinCore.h" // for wait_for_user
#include "../core/types.h"
#include "../gcode/gcode.h"
#include "../libs/buzzer.h"
#include "../lcd/marlinui.h"

#include "../lcd/e3v2/proui/dwinui.h" // mmm
#include "../lcd/e3v2/proui/dwin_lcd.h" // mmm
#include "custom_gcodes.h"
#include "../lcd/e3v2/proui/toolbar.h" // mmm
#include "../core/macros.h" // mmm
#include "../module/planner.h" // mmm

#if HAS_MEDIA
  #include "file_header.h"
#endif

#ifdef DWIN_LCD_PROUI
  #include "../lcd/e3v2/proui/dwin.h"
  #if ENABLED(LCD_BED_TRAMMING)
    #include "../lcd/e3v2/proui/bed_tramming.h"
  #endif
#endif

#include "custom_gcodes.h"


//=============================================================================
// Extended G-CODES
//=============================================================================

void cError() {
  parser.unknown_command_warning();
}

#if HAS_CUSTOM_COLORS
// C11 Set color for UI element E
  void C11() {
    const int16_t E = parser.seenval('E') ? parser.value_byte() : 0; // UI element
    if (E) {
      hmiValue.Color[0] = parser.seenval('R') ? parser.value_byte() : 0;
      hmiValue.Color[1] = parser.seenval('G') ? parser.value_byte() : 0;
      hmiValue.Color[2] = parser.seenval('B') ? parser.value_byte() : 0;
      dwinApplyColor(E);
    } else dwinRedrawScreen();
  }
#endif

#if ALL(HAS_BED_PROBE, HAS_TRAMMING_WIZARD)
  // Bed tramming
  void C35() {
    if (parser.seenval('T')) {
      const int8_t i = parser.value_byte();
      if (WITHIN(i, 0, 4)) tram(i);
    }
    TERN_(HAS_TRAMMING_WIZARD, else runTrammingWizard());
  }
#endif

// Cancel a Wait for User without an Emergecy Parser
void C108() {
  #if DEBUG_DWIN
    SERIAL_ECHOLNPGM(F("wait_for_user was "), wait_for_user);
    SERIAL_ECHOLNPGM(F("checkkey was "), checkkey);
  #endif
  #if LCD_BACKLIGHT_TIMEOUT_MINS
    ui.refresh_backlight_timeout();
  #endif
  #if HAS_LCD_BRIGHTNESS
    if (!ui.backlight) ui.refresh_brightness();
  #endif
  wait_for_user = false;
  DONE_BUZZ(true);
}

// Enable or disable preview screen
#if ALL(HAS_GCODE_PREVIEW, PREVIEW_MENU_ITEM)
void C250() {
  if (parser.seenval('P')) {
    hmiData.enablePreview = !!parser.value_byte();
  }
  SERIAL_ECHOLNPGM(F("PREVIEW:"), hmiData.enablePreview);
}
#endif

// lock/unlock screen
#if HAS_LOCKSCREEN
  void C510() {
    if (parser.seenval('U') && parser.value_int()) dwinUnLockScreen();
    else dwinLockScreen();
  }
#endif

#if ENABLED(DEBUG_DWIN)
  void C997() {
    SERIAL_ECHOLNPGM("Simulating a printer freeze");
    TERN_(DWIN_LCD_PROUI, dwinRebootScreen());
    while (1) {};
  }
#elif NONE(PLATFORM_M997_SUPPORT, DWIN_LCD_PROUI)
  void C997() {
    SERIAL_ECHOLNPGM("Reboot printer");
    hal.reboot();
  }
#endif

// Special Creality DWIN GCodes
void customGcode(const int16_t codenum) {
  switch(codenum) {
    #if HAS_CUSTOM_COLORS
      case 11: C11(); break;              // Set color for UI element E
    #endif
    #if ALL(HAS_BED_PROBE, HAS_TRAMMING_WIZARD)
      case 35: C35(); break;              // Launch bed tramming wizard
    #endif
    case 108: C108(); break;              // Cancel a Wait for User without an Emergecy Parser
    #if HAS_LOCKSCREEN
      case 510: C510(); break;            // lock screen
    #endif
    #if ENABLED(DEBUG_DWIN) || NONE(PLATFORM_M997_SUPPORT, DWIN_LCD_PROUI)
      case 997: C997(); break;            // Simulate a printer freeze (or reboot)
    #endif
    #if HAS_MEDIA
      case 10: proUIEx.C10(); break;    // Mark the G-code file as a Configuration file
    #endif
    #if HAS_MESH
      case 29: proUIEx.C29(); break;    // Set probing area and mesh leveling settings
    #endif
    case 100: proUIEx.C100(); break;    // Change Physical minimums
    case 101: proUIEx.C101(); break;    // Change Physical maximums
    case 102: proUIEx.C102(); break;    // Change Bed size
    #if HAS_PROUI_MAXTEMP
      case 104: proUIEx.C104(); break;    // Set extruder max temperature (limited by maxtemp in thermistor table)
    #endif
    case 115: proUIEx.C115(); break;    // ProUI Info
    #if ENABLED(NOZZLE_PARK_FEATURE)
      case 125: proUIEx.C125(); break;  // Set park position
    #endif
    #if ALL(HAS_GCODE_PREVIEW, PREVIEW_MENU_ITEM)
      case 250: C250(); break;          // Enable or disable preview screen
    #endif
    #if ANY(PIDTEMP,PIDTEMPBED) // mmm
      case 303: C303(); break;            // set PID temp and cycles for Bed or Hotend
    #endif // mmm
    #if HAS_FILAMENT_SENSOR
      case 412: proUIEx.C412(); break;  // Set runout sensor active mode
    #endif
    case 562: proUIEx.C562(); break;    // Invert Extruder
    case 851: proUIEx.C851(); break;    // If has a probe set z feed rate and multiprobe, if not, set manual z-offset
    #if HAS_TOOLBAR
      case 810: C810(); break;            // mmm Config toolbar
    #endif
      case 120: C120(); break;              // visual setting T = hms, F = fan%, I = iconset
    #if ENABLED(LASER_FAN_SHARING)
      case 3:   C3(); break;                // set L = laser mode, F = fan mode
    #endif
    default: cError(); break;
  }
}

void customGcodeReport(const bool forReplay/*=true*/) {
  proUIEx.C100_report(forReplay);
  proUIEx.C101_report(forReplay);
  proUIEx.C102_report(forReplay);
  #if HAS_MESH
    proUIEx.C29_report(forReplay);
  #endif
  #if HAS_PROUI_MAXTEMP
    proUIEx.C104_report(forReplay);
  #endif
  #if ENABLED(NOZZLE_PARK_FEATURE)
    proUIEx.C125_report(forReplay);
  #endif
      #if ANY(PIDTEMP,PIDTEMPBED) // mmm
        C303_report();
      #endif // mmm
  #if HAS_FILAMENT_SENSOR
    proUIEx.C412_report(forReplay);
  #endif
    proUIEx.C562_report(forReplay);
  #if HAS_BED_PROBE
    proUIEx.C851_report(forReplay);
      C120_report(); // mmm
    #if ENABLED(LASER_FAN_SHARING)
      C3_report();
    #endif
      C120_report(); // mmm
    #if ENABLED(LASER_FAN_SHARING)
      C3_report();
    #endif
  #endif
}

#if ANY(PIDTEMP,PIDTEMPBED) // mmm
  void C303() {
    if (parser.seen("CES")) {
      // set heater
      uint8_t heater = 0;
      if (parser.seenval('E')) heater = parser.byteval('E');
      if (parser.seenval('S')) (heater==-1 ? hmiData.bedPidT : hmiData.hotendPidT)= parser.byteval('O');
      if (parser.seenval('C')) hmiData.pidCycles = parser.byteval('C');
      return;
    }
    C3_report();
  }

  void C303_report(const bool forReplay/*=true*/) {
    gcode.report_heading(forReplay, F("PID Configuration"));
    gcode.report_echo_start(forReplay);
    SERIAL_ECHOPGM("  C303");
    SERIAL_ECHOPGM(" E-1", hmiData.bedPidT);
    SERIAL_ECHOPGM(" C", hmiData.pidCycles);
    SERIAL_EOL();
  }
#endif


#if ENABLED(LASER_FAN_SHARING)
  void C3 () {
    /* C3 - Laser Mode enable/disable 
          L - enable Laser mode. commands G3-5 enabled and fan changes sync with movement
          F - enable Fan mode. G3-5 disabled. fan changes immediately
          fan has priority if both selected
    */
    if (parser.seen("FHLO")) {
      // set to fan mode
      if (parser.seen("F")) setLaserMode(false);
      // Set to Laser mode
      else if (parser.seen("L")) setLaserMode(true);
      // Set Laser Off Limit. used in G0 moves to keep laser active but not burning.
      if (parser.seenval('O')) hmiData.laser_off_pwr = parser.byteval('O');
      if (parser.seenval('H')) hmiData.target_laser_height = parser.byteval('H');
      return;
    }
    C3_report();
  }

  void C3_report(const bool forReplay/*=true*/) {
    gcode.report_heading(forReplay, F("Laser/Fan Configuration"));
    gcode.report_echo_start(forReplay);
    SERIAL_ECHOPGM("  C3");
    SERIAL_ECHOPGM(" O", hmiData.laser_off_pwr);
    SERIAL_ECHOPGM(" H", hmiData.target_laser_height);
    if (planner.laserMode)
      SERIAL_ECHOPGM(" L  ; Laser Mode enabled");
    else
      SERIAL_ECHOPGM(" F  ; Fan Mode enabled");
    SERIAL_EOL();
  }
#endif


void C120 () {
  /* C120 - Display cofiguration settings 
          In - set icon set n. stock and default is 9 
          Tn - n : 1 - enable time in hms format. 0 - use : formate for time
          Fn - n : 1 - display fan speed in percent. 0 - display raw fan speed value 
  */
  if (parser.seen("ITFS")) {
    if (parser.seenval('I')) { hmiData.baseIcon = parser.byteval('I'); dwinRedrawScreen(); }
    if (parser.seenval('T')) { hmiData.time_format_textual = parser.boolval('T'); }
    if (parser.seenval('S')) { hmiData.autoStoreSD = parser.boolval('S'); }
    return;
  }
  C120_report(true);
}

void C120_report(const bool forReplay/*=true*/) {
  gcode.report_heading(forReplay, F("Display Config Settings"));
  gcode.report_echo_start(forReplay);
  SERIAL_ECHOPGM("  C120 I", hmiData.baseIcon);
  SERIAL_ECHOPGM(" T", hmiData.time_format_textual? 1:0);
  SERIAL_ECHOPGM(" S", hmiData.autoStoreSD? 1:0);
  SERIAL_EOL();
}

void C810 () {
  /* C810 - ToolbarSetup
  */
  if (parser.seen("ABCDE")) {
    uint8_t tbMax = toolBar.OptCount();
    if (parser.seenval('A'))  PRO_data.TBopt[0] = _MIN(parser.intval('A'),tbMax); 
    if (parser.seenval('B'))  PRO_data.TBopt[1] = _MIN(parser.intval('B'),tbMax); 
    if (parser.seenval('C'))  PRO_data.TBopt[2] = _MIN(parser.intval('C'),tbMax); 
    if (parser.seenval('D'))  PRO_data.TBopt[3] = _MIN(parser.intval('D'),tbMax); 
    if (parser.seenval('E'))  PRO_data.TBopt[4] = _MIN(parser.intval('E'),tbMax);
    return;
  }
  C810_report(true);
}
void C810_report(const bool forReplay/*=true*/) {
  gcode.report_heading(forReplay, F("Display Config Settings"));
  gcode.report_echo_start(forReplay);
  SERIAL_ECHOPGM("  C810 A", PRO_data.TBopt[0]);
  SERIAL_ECHOPGM(" B", PRO_data.TBopt[1]);
  SERIAL_ECHOPGM(" C", PRO_data.TBopt[2]);
  SERIAL_ECHOPGM(" D", PRO_data.TBopt[3]);
  SERIAL_ECHOPGM(" E", PRO_data.TBopt[4]);
  SERIAL_EOL();
}


#endif // PROUI_EX && HAS_CGCODE
