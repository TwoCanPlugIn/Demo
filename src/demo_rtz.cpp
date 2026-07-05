// Copyright(C) 2026 by Steven Adler
//
// This file is part of the Demo plugin for OpenCPN.
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
//

//
// Project: GPX Export
// Description: Derived from Demo plugin, demonstrate export of routes formatted using RTZ
// Owner: twocanplugin@hotmail.com
// Date: 31/03/2026
// Version History: 
// 1.0 Initial Release
// 03/05/2026 - 1.1 Export waypoints using GeoJSON format
// 14/05/2026 - 1.2 Export Waypoints to SignalK
// 30/06/2026 - 1.3 Export Routes using RTZ
// 01/07/2026 - 1.4 Export Routes using S-421

#include "demo_rtz.h"

DemoRTZ::DemoRTZ() {
    // Nothing to do. XML document is generated in CreateRoute function
}

void DemoRTZ::CreateRoute(const std::string& routeName,
    const std::string& vesselName,
    const std::string& author,
    const std::string& status) {
    doc.reset();

    pugi::xml_node route = doc.append_child("route");
    route.append_attribute("version") = "1.2";
    route.append_attribute("xmlns") = "http://www.cirm.org/RTZ/1/2";
    pugi::xml_node routeInfo = route.append_child("routeInfo");
    routeInfo.append_attribute("routeName") = routeName.c_str();
    routeInfo.append_attribute("routeAuthor") = author.c_str();
    routeInfo.append_attribute("routeStatus") = status.c_str();
    routeInfo.append_attribute("vesselName") = "Evergiven";

    route.append_child("waypoints");
    route.append_child("schedules");
}

void DemoRTZ::AddWaypoint(int id,
    int revision,
    double lat,
    double lon,
    const std::string& name,
    double radius,
    const LegAttributes& leg) {
    pugi::xml_node wps = doc.child("route").child("waypoints");
    pugi::xml_node w = wps.append_child("waypoint");

    w.append_attribute("id") = id;
    w.append_attribute("revision") = revision;

    if (!name.empty()) {
        w.append_attribute("name") = name.c_str();
    }

    if (radius >= 0) {
        w.append_attribute("radius") = radius;
    }

    pugi::xml_node pos = w.append_child("position");
    pos.append_attribute("lat") = lat;
    pos.append_attribute("lon") = lon;

    AddLeg(w, leg);
}

pugi::xml_node DemoRTZ::AddLeg(pugi::xml_node parent, const LegAttributes& leg) {
    pugi::xml_node l = parent.append_child("leg");

    auto addAttr = [&](const char* name, double value) {
        if (value >= 0) {
            l.append_attribute(name) = value;
        }
    };

    addAttr("starboardXTD", leg.starboardXTD);
    addAttr("portsideXTD", leg.portsideXTD);
    addAttr("safetyContour", leg.safetyContour);
    addAttr("safetyDepth", leg.safetyDepth);

    if (!leg.geometryType.empty()) {
        l.append_attribute("geometryType") = leg.geometryType.c_str();
    }

    addAttr("speedMin", leg.speedMin);
    addAttr("speedMax", leg.speedMax);
    addAttr("draughtForward", leg.draughtForward);
    addAttr("draughtAft", leg.draughtAft);
    addAttr("staticUKC", leg.staticUKC);
    addAttr("dynamicUKC", leg.dynamicUKC);
    addAttr("masthead", leg.masthead);

    if (!leg.legReport.empty()) {
        l.append_attribute("legReport") = leg.legReport.c_str();
    }
    if (!leg.legInfo.empty()) {
        l.append_attribute("legInfo") = leg.legInfo.c_str();
    }
    if (!leg.legNote1.empty()) {
        l.append_attribute("legNote1") = leg.legNote1.c_str();
    }
    if (!leg.legNote2.empty()) {
        l.append_attribute("legNote2") = leg.legNote2.c_str();
    }

    return l;
}

void DemoRTZ::AddSchedule(const Schedule& schedule) {
    pugi::xml_node schs = doc.child("route").child("schedules");
    pugi::xml_node s = schs.append_child("schedule");

    s.append_attribute("id") = schedule.id;

    if (!schedule.name.empty()) {
        s.append_attribute("name") = schedule.name.c_str();
    }

    if (!schedule.manual.empty()) {
        pugi::xml_node manual = s.append_child("manual");
        for (const auto& se : schedule.manual) {
            AddScheduleElement(manual, se);
        }
    }

    if (!schedule.calculated.empty()) {
        pugi::xml_node calc = s.append_child("calculated");
        for (const auto& se : schedule.calculated) {
            AddScheduleElement(calc, se);
        }
    }
}

pugi::xml_node DemoRTZ::AddScheduleElement(pugi::xml_node parent, const ScheduleElement& se) {
    pugi::xml_node s = parent.append_child("scheduleElement");

    s.append_attribute("waypointId") = se.waypointId;

    if (!se.etd.empty()) {
        s.append_attribute("etd") = se.etd.c_str();
    }
    if (!se.eta.empty()) {
        s.append_attribute("eta") = se.eta.c_str();
    }
    if (!se.stay.empty()) {
        s.append_attribute("stay") = se.stay.c_str();
    }
    if (se.speed >= 0) {
        s.append_attribute("speed") = se.speed;
    }

    return s;
}

bool DemoRTZ::Save(const std::string& filename) {
    return doc.save_file(filename.c_str(), "  ");
}
