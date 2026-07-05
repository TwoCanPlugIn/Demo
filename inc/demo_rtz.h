// Copyright(C) 2026 by Steven Adler
//
// This file is part of Demo plugin for OpenCPN.
//
// Demo plugin for OpenCPN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Demo plugin for OpenCPN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Demo plugin for OpenCPN. If not, see <https://www.gnu.org/licenses/>.


#ifndef DEMO_RTZ_H
#define DEMO_RTZ_H

#include <pugixml.hpp>
#include <string>
#include <vector>

struct LegAttributes {
    double starboardXTD = -1;
    double portsideXTD = -1;
    double safetyContour = -1;
    double safetyDepth = -1;
    std::string geometryType; // "Loxodrome" or "Orthodrome"
    double speedMin = -1;
    double speedMax = -1;
    double draughtForward = -1;
    double draughtAft = -1;
    double staticUKC = -1;
    double dynamicUKC = -1;
    double masthead = -1;
    std::string legReport;
    std::string legInfo;
    std::string legNote1;
    std::string legNote2;
};

struct ScheduleElement {
    int waypointId;
    std::string etd;
    std::string eta;
    std::string stay;
    double speed = -1;
};

struct Schedule {
    int id;
    std::string name;
    std::vector<ScheduleElement> manual;
    std::vector<ScheduleElement> calculated;
};

class DemoRTZ
{
public:
    DemoRTZ();

    void CreateRoute(const std::string& routeName, 
        const std::string& vesselName,
        const std::string& author = "OpenCPN",
        const std::string& status = "Planned");

    void AddWaypoint(int id,
        int revision,
        double lat,
        double lon,
        const std::string& name = "",
        double radius = -1,
        const LegAttributes& leg = {});

    void AddSchedule(const Schedule& schedule);

    bool Save(const std::string& filename);

private:
    pugi::xml_document doc;

    pugi::xml_node AddLeg(pugi::xml_node parent, const LegAttributes& leg);
    pugi::xml_node AddScheduleElement(pugi::xml_node parent, const ScheduleElement& se);
};

#endif