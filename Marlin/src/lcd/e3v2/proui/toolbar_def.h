/**
 * toolBar for PRO UI
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * version: 2.1.1
 * Date: 2023/09/12
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

#pragma once

#include "../../../inc/MarlinConfig.h"

#include "dwin.h"
#include "toolbar.h"

#define TB_ITEM(I,L,V...) {I, GET_TEXT_F(L), V}

const TBItem_t TBItemA[] = {
  {0, GET_TEXT_F(MSG_OPTION_DISABLED), nullptr},
  {ICON_Homing, GET_TEXT_F(MSG_AUTO_HOME), autoHome},
  {ICON_HomeXY, GET_TEXT_F(MSG_AUTO_HOME_XY), homeXY}, // mmm
  #if HAS_BED_PROBE
    #if HAS_TRAMMING_WIZARD
      TB_ITEM(ICON_BedTramming, MSG_TRAMMING_WIZARD, runTrammingWizard),
    #endif
    TB_ITEM(ICON_SetZOffset, MSG_PROBE_WIZARD, drawZOffsetWizMenu),
    TB_ITEM(ICON_Level, MSG_AUTO_MESH, autoLev),
  #else
    TB_ITEM(ICON_MoveZ0, MSG_HOME_Z_AND_DISABLE, homeZandDisable),
  #endif
  TB_ITEM(ICON_CloseMotor, MSG_DISABLE_STEPPERS, disableMotors),
  TB_ITEM(ICON_Cool, MSG_COOLDOWN, doCoolDown),
  #if HAS_PREHEAT
    #define _TBPREHEAT(N) TB_ITEM(ICON_Preheat##N, MSG_PREHEAT_##N, DoPreheat##N),
    REPEAT_1(PREHEAT_COUNT, _TBPREHEAT)
  #endif
  #if HAS_FILAMENT_SENSOR // mmm
    {ICON_Runout, GET_TEXT_F(MSG_RUNOUT_TOGGLE), toggleRunout},
  #endif
    {ICON_FilUnload,GET_TEXT_F(MSG_FILAMENT_MGT), drawFilamentManMenu},
  #if ENABLED(HOST_SHUTDOWN_MENU_ITEM) && defined(SHUTDOWN_ACTION)
    {ICON_Host, GET_TEXT_F(MSG_HOST_SHUTDOWN), hostShutDown},
  #endif
  {ICON_Brightness, GET_TEXT_F(MSG_BRIGHTNESS_OFF), turnOffBacklight},
  {ICON_Reboot, GET_TEXT_F(MSG_RESET_PRINTER), rebootPrinter},
  {ICON_WriteEEPROM, GET_TEXT_F(MSG_STORE_EEPROM), writeEeprom},
  #if ENABLED(LASER_FAN_SHARING) // ...
    {ICON_LaserMode, GET_TEXT_F(MSG_TOGGLE_LASER),toggleLaserMode},
  #endif
  {ICON_Park, GET_TEXT_F(MSG_FILAMENT_PARK_ENABLED), parkHead}
};
