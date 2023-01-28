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
#include "dwin.h"
#include "proui.h"
#include "../../marlinui.h"
#include "../../../libs/buzzer.h"

void WriteSDConfig() {
  String buffer = {};
  card.openFileWrite("config.gcode");
    // C10                    ; Mark as a configuration file
  buffer = "; Professional Firmware Configuration File\n";
  buffer += ";=========================================\n";
  buffer += "C10\n";
  buffer += "M117 Applying configuration\n";
  // ; reset settings to defaults
  buffer += "M502\n";

  // ; physical settings
  buffer += "C100X"+ String(PRO_data.x_min_pos) + "Y" + String(PRO_data.y_min_pos) + "\n";
  buffer += "C101X"+ String(PRO_data.x_max_pos) + "Y" + String(PRO_data.y_max_pos) + "Z" + String(PRO_data.z_max_pos) + "\n";
  buffer += "C102X"+ String(PRO_data.x_bed_size) + "Y" + String(PRO_data.y_bed_size) + "\n";
  buffer += "C125X"+ String(PRO_data.Park_point.x) + "Y" + String(PRO_data.Park_point.y) + "Z" + String(PRO_data.Park_point.z) + "\n";

  // ; Bed and Hotend specific changes
  buffer += "C104U1T" + String(PRO_data.hotend_maxtemp) + "\n";
  MPC_t *pMpc = &thermalManager.temp_hotend[0].constants;
  buffer += "M306C" + String(pMpc->block_heat_capacity) +"R"+ String(pMpc->sensor_responsiveness,4) + "A"+ String(pMpc->ambient_xfer_coeff_fan0,4) + "F" + String(pMpc->ambient_xfer_coeff_fan0+pMpc->fan255_adjustment,4) + "\n";
  bed_info_t *pBed = &thermalManager.temp_bed;
  buffer += "M304P" + String(thermalManager.temp_bed.pid.p()) + "I" + String(thermalManager.temp_bed.pid.i()) + "D" + String(thermalManager.temp_bed.pid.d()) + "\n";

  // ; Probe settings
  buffer += "C29L" + String(PRO_data.mesh_min_x) + "R" + String(PRO_data.mesh_max_x) + "F" + String(PRO_data.mesh_min_x) + "B" + String(PRO_data.mesh_max_y) + "N" + String(PRO_data.grid_max_points) + "\n";
  buffer += "M851X" + String(probe.offset.x) + "Y" + String(probe.offset.y) + "Z" + String(probe.offset.z) + "\n";
  buffer += "C851S" + String(PRO_data.zprobefeedslow) + "M" + String(PRO_data.multiple_probing) + "\n";

  // ; filament sensor and runout
  buffer += "C412M" + String(PRO_data.Runout_active_state) + "S" + String(runout.enabled) + "D" + String(runout.runout_distance()) + "\n";

  // ; Exttruder settings
  buffer += "C562E" + String(PRO_data.Invert_E0? 1:0) + "\n";
  buffer += "M92E" + String(planner.settings.axis_steps_per_mm[E_AXIS]) + "X" + String(planner.settings.axis_steps_per_mm[X_AXIS]) + "Y" +String(planner.settings.axis_steps_per_mm[Y_AXIS]) + "Z" + String(planner.settings.axis_steps_per_mm[Z_AXIS]) + "\n";
  buffer += "M603L" + String(fc_settings[0].load_length) + "U" + String (fc_settings[0].unload_length) + "P" + String(fc_settings[0].unload_prelength) + "D" + String(fc_settings[0].unload_predelay) + "\n";

  // ; fwretraction
  buffer += "M207F" + String(60*fwretract.settings.retract_feedrate_mm_s) + "S" + String(fwretract.settings.retract_length) + "Z" + String(fwretract.settings.retract_zraise) + "\n";
  buffer += "M208F" + String(60*fwretract.settings.retract_recover_feedrate_mm_s) + "S" + String(fwretract.settings.retract_recover_extra) + "\n";

  // ; customization
  buffer += "C810A" + String(PRO_data.TBopt[0]) + "B" + String(PRO_data.TBopt[1])+ "C" + String(PRO_data.TBopt[2])+ "D" + String(PRO_data.TBopt[3])+ "E" + String(PRO_data.TBopt[4]) +"\n";
  buffer += "M256B" + String(ui.brightness) + "\n";
  buffer += "M413S" + String(recovery.enabled) + "\n";
  buffer += "C120I" + String(HMI_data.baseIcon) + "T" + String(HMI_data.time_format_textual? 1:0) + "F" + String(HMI_data.fan_percent? 1:0) + "\n";
  buffer += "C3FO" + String(HMI_data.laser_off_pwr) + "H" + String(HMI_data.target_laser_height) + "\n";

  // ; Save
  buffer += "M500\nG4S1\nM300P200\n";
  buffer += "M117 Configuration Applied\n";

  //TODO: Create Mesh
  //TODO: ui colors

  char charbuff[buffer.length()] = {0};
  buffer.toCharArray(charbuff,buffer.length());

  card.write(charbuff,buffer.length());
  card.closefile();
  DONE_BUZZ(true);
  ui.status_printf(0,F("Config GCode Stored"));
}