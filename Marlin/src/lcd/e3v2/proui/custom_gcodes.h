/**
 * Custom G-code implementation for PRO UI
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 2.1.0
 * Date: 2023/07/12
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

void customGcode(const int16_t codenum);
void customGcode_report(const bool forReplay=true);

    void C810(); // mmm
    void C810_report(const bool forReplay=true);
    void C120();
    void C120_report(const bool forReplay=true);
  #if ANY(PIDTEMP,PIDTEMPBED)
    void C303();
    void C303_report(const bool forReplay=true);
  #endif
  #if ENABLED(LASER_FAN_SHARING)
    void C3();
    void C3_report(const bool forReplay=true);
  #endif
