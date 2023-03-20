/**
 * Configuration Storing to SD for easy migrations
 * Author: Matthew Coon
 * Version: 1.0
 * Date: 2023/01/18
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

#include "../../../inc/MarlinConfig.h"
#include "../../../sd/cardreader.h"
#include "../../../module/temperature.h"
#include "../../../module/probe.h"
#include "../../../feature/runout.h"
#include "../../../feature/powerloss.h"
#include "../../../feature/bltouch.h"
#include "dwin.h"
#include "proui.h"
#include "../../marlinui.h"
#include "../../../libs/buzzer.h"
#include "../../../feature/bedlevel/bedlevel.h"


void WriteMeshConfig (String *lBuffer) {
  int8_t i,j;
  for (i = 0;i<GRID_LIMIT;i++) {
    for (j = 0;j<GRID_LIMIT;j++) {
      *lBuffer += "M421I" + String(i) + "J" + String(j) + "Z" + String(bedlevel.z_values[i][j]) + "\n";
    }
  }
}

void WriteMaterialConfig(String *lBuffer) {
  uint8_t i = 0;
  for(i = 0; i < PREHEAT_COUNT; i++) {
    *lBuffer += "M145 S" + String(i) + "H" + String(ui.material_preset[i].hotend_temp) + "B" + String(ui.material_preset[i].bed_temp) + "F" + String(ui.material_preset[i].fan_speed) + "\n";
  }
}


void WriteRGBConfig (String *lBuffer,uint8_t cIndex, uint16_t color) {
  *lBuffer += "C11E"+ String(cIndex) +"R" + String(GetRColor(color)) + "G" + String(GetGColor(color)) + "B" + String(GetBColor(color)) + "\n";
}

void WriteSDConfig() {
  #if ProUIex
    String buffer = {};
    card.openFileWrite("config.gcode");
      // C10                    ; Mark as a configuration file
    buffer = "; Professional Firmware Configuration File\n";
    buffer += ";=====================================================\n";
    buffer += "C10 \n";
    buffer += "M117 Applying configuration\n";
    buffer += ";-----------------------------------------------------\n";
    // reset settings to defaults
    buffer += "M502\n";

    // physical settings
    buffer += "C100X"+ String(PRO_data.x_min_pos) + "Y" + String(PRO_data.y_min_pos) + "\n";
    buffer += "C101X"+ String(PRO_data.x_max_pos) + "Y" + String(PRO_data.y_max_pos) + "Z" + String(PRO_data.z_max_pos) + "\n";
    buffer += "C102X"+ String(PRO_data.x_bed_size) + "Y" + String(PRO_data.y_bed_size) + "\n";
    buffer += "C125X"+ String(PRO_data.Park_point.x) + "Y" + String(PRO_data.Park_point.y) + "Z" + String(PRO_data.Park_point.z) + "\n";

    // movement settings
    buffer += "M205B" + String(planner.settings.min_segment_time_us) + "S" + String(planner.settings.min_feedrate_mm_s)+ "T" + String(planner.settings.min_travel_feedrate_mm_s);
    #if HAS_JUNCTION_DEVIATION
      buffer += "J" + String(planner.junction_deviation_mm);
    #endif
    #if HAS_CLASSIC_JERK
      buffer += "E" + String(planner.max_jerk.e) + "X" + String(planner.max_jerk.x) + "Y" +String(planner.max_jerk.y) + "Z" + String(planner.max_jerk.e);
    #endif
    buffer += "\n";

    // Bed and Hotend specific changes
    buffer += "C104U1T" + String(PRO_data.hotend_maxtemp) + "\n";
    buffer += "M302S" + String(HMI_data.ExtMinT) + "\n";

    #if ENABLED(MPCTEMP)
      MPC_t *pMpc = &thermalManager.temp_hotend[0].mpc;
      buffer += "M306C" + String(pMpc->block_heat_capacity) +"R"+ String(pMpc->sensor_responsiveness,4) + "A"+ String(pMpc->ambient_xfer_coeff_fan0,4) + "F" + String(pMpc->ambient_xfer_coeff_fan0+pMpc->fan255_adjustment,4) + "\n";
    #endif
    #if ENABLED(PIDTEMP)
      buffer += "C303E0C" + String(HMI_data.PidCycles) + "S" + String(HMI_data.HotendPidT) + "\n";
    #endif
  // int16_t HotendPidT = DEF_HOTENDPIDT;
    #if ENABLED(PIDTEMPBED)
      buffer += "C303E-1C" + String(HMI_data.PidCycles) + "S" + String(HMI_data.BedPidT) + "\n";
      buffer += "M304P" + String(thermalManager.temp_bed.pid.p()) + "I" + String(thermalManager.temp_bed.pid.i()) + "D" + String(thermalManager.temp_bed.pid.d()) + "\n";
    #endif

    WriteMaterialConfig(&buffer);

    // ; Probe settings
    buffer += "C29L" + String(PRO_data.mesh_min_x) + "R" + String(PRO_data.mesh_max_x) + "F" + String(PRO_data.mesh_min_x) + "B" + String(PRO_data.mesh_max_y) + "N" + String(PRO_data.grid_max_points) + "\n";
    #if HAS_BED_PROBE
      buffer += "M851X" + String(probe.offset.x) + "Y" + String(probe.offset.y) + "Z" + String(probe.offset.z) + "\n";
      buffer += "C851S" + String(PRO_data.zprobefeedslow) + "M" + String(PRO_data.multiple_probing) + "\n";
      buffer += "M401S" + String(bltouch.high_speed_mode? 1:0) + "\n";
    #endif

    // ; filament sensor and runout
    #if HAS_FILAMENT_SENSOR
      buffer += "C412M" + String(PRO_data.Runout_active_state) + "S" + String(runout.enabled) + "D" + String(runout.runout_distance()) + "\n";
    #endif

    // ; Exttruder settings
    buffer += "C562E" + String(PRO_data.Invert_E0? 1:0) + "\n";
    buffer += "M92E" + String(planner.settings.axis_steps_per_mm[E_AXIS]) + "X" + String(planner.settings.axis_steps_per_mm[X_AXIS]) + "Y" +String(planner.settings.axis_steps_per_mm[Y_AXIS]) + "Z" + String(planner.settings.axis_steps_per_mm[Z_AXIS]) + "\n";
    #if ENABLED(ADVANCED_PAUSE_FEATURE)
      buffer += "M603L" + String(fc_settings[0].load_length) + "U" + String (fc_settings[0].unload_length) + "P" + String(fc_settings[0].unload_prelength) + "D" + String(fc_settings[0].unload_predelay) + "\n";
    #endif

    // ; fwretraction
    #if ENABLED(FWRETRACT)
      buffer += "M207F" + String(60*fwretract.settings.retract_feedrate_mm_s) + "S" + String(fwretract.settings.retract_length) + "Z" + String(fwretract.settings.retract_zraise) + "\n";
      buffer += "M208F" + String(60*fwretract.settings.retract_recover_feedrate_mm_s) + "S" + String(fwretract.settings.retract_recover_extra) + "\n";
    #endif

    // ; customization
    buffer += "C810A" + String(PRO_data.TBopt[0]) + "B" + String(PRO_data.TBopt[1])+ "C" + String(PRO_data.TBopt[2])+ "D" + String(PRO_data.TBopt[3])+ "E" + String(PRO_data.TBopt[4]) +"\n";
    #if HAS_LCD_BRIGHTNESS
      buffer += "M256B" + String(ui.brightness) + "\n";
    #endif
    #if ENABLED(POWER_LOSS_RECOVERY)
      buffer += "M413S" + String(recovery.enabled) + "\n";
    #endif
    buffer += "C120I" + String(HMI_data.baseIcon) + "T" + String(HMI_data.time_format_textual? 1:0) + "F" + String(HMI_data.fan_percent? 1:0) + "\n";
    #if ENABLED(LASER_FAN_SHARING)
      buffer += "C3FO" + String(HMI_data.laser_off_pwr) + "H" + String(HMI_data.target_laser_height) + "\n";
    #endif

    // store current mesh as M421 commands
    WriteMeshConfig(&buffer);

    // ui colors
    // C11 E2  R1 G2 B1     ;Screen Background
    WriteRGBConfig(&buffer,2,HMI_data.Background_Color);
    WriteRGBConfig(&buffer,3,HMI_data.Cursor_Color);
    WriteRGBConfig(&buffer,4,HMI_data.TitleBg_Color);
    WriteRGBConfig(&buffer,5,HMI_data.TitleTxt_Color);
    WriteRGBConfig(&buffer,6,HMI_data.Text_Color);
    WriteRGBConfig(&buffer,7,HMI_data.Selected_Color);
    WriteRGBConfig(&buffer,8,HMI_data.SplitLine_Color);
    WriteRGBConfig(&buffer,9,HMI_data.Highlight_Color);
    WriteRGBConfig(&buffer,10,HMI_data.StatusBg_Color);
    WriteRGBConfig(&buffer,11,HMI_data.StatusTxt_Color);
    WriteRGBConfig(&buffer,12,HMI_data.PopupBg_Color);
    WriteRGBConfig(&buffer,13,HMI_data.PopupTxt_Color);
    WriteRGBConfig(&buffer,14,HMI_data.AlertBg_Color);
    WriteRGBConfig(&buffer,15,HMI_data.AlertTxt_Color);
    WriteRGBConfig(&buffer,16,HMI_data.PercentTxt_Color);
    WriteRGBConfig(&buffer,17,HMI_data.Barfill_Color);
    WriteRGBConfig(&buffer,18,HMI_data.Indicator_Color);
    WriteRGBConfig(&buffer,19,HMI_data.Coordinate_Color);

    // ; Save
    buffer += "M500\nG4S1\nM300P200\n";
    buffer += "M117 Configuration Applied\n";

    char charbuff[buffer.length()] = {0};
    buffer.toCharArray(charbuff,buffer.length());

    card.write(charbuff,buffer.length());
    card.closefile();
    DONE_BUZZ(true);
    ui.status_printf(0,F("Config GCode Stored"));
  #endif
}

  // bool AdaptiveStepSmoothing = true;
  // int16_t BedLevT = LEVELING_BED_TEMP;
  // TERN_(BAUD_RATE_GCODE, HMI_data.Baud115K = (BAUDRATE == 115200));
  // HMI_data.FullManualTramming = false;
  // HMI_data.MediaAutoMount = ENABLED(HAS_SD_EXTENDER);
