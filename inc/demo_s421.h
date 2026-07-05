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


#ifndef DEMO_S421_H
#define DEMO_S421_H

#pragma once
#include <pugixml.hpp>
#include <string>
#include <vector>

// -----------------------------
// S-421 Basic Feature Structures
// -----------------------------
struct S421Waypoint {
	int id;
	double lat;
	double lon;
	std::string name;
};

struct S421ScheduleElement {
	int waypointId;
	std::string etd;
	std::string eta;
	double speed;
};

struct S421Schedule {
	int id;
	std::string name;
	std::vector<S421ScheduleElement> manual;
	std::vector<S421ScheduleElement> calculated;
};

// -----------------------------
// S-100 Metadata
// -----------------------------
struct S100Metadata {
	std::string productSpecification;
	std::string issueDate;
	std::string producingAgency;

	double minLat = 0;
	double minLon = 0;
	double maxLat = 0;
	double maxLon = 0;
};

// -----------------------------
// demo_s421 Class
// -----------------------------
class DemoS421
{
public:
	DemoS421();

	// Dataset root
	void CreateDataset(const std::string& datasetIdentifier);

	// Metadata
	void AddS100Metadata(const S100Metadata& meta);

	// S-421 Features
	void AddRouteInfo(const std::string& name,
		const std::string& author,
		const std::string& status);

	void AddWaypoint(const S421Waypoint& wp);
	void AddSchedule(const S421Schedule& schedule);

	// Save XML
	bool SaveXML(const std::string& filename);

	// Save HDF5 (GDAL-compatible)
	bool SaveHDF5(const std::string& filename);

private:
	pugi::xml_document doc;
	pugi::xml_node featureContainer;

	int nextFeatureId = 1;

	// Helpers
	std::string MakeFeatureID(const std::string& prefix);
	pugi::xml_node AddGMLPoint(double lat, double lon);
	pugi::xml_node AddReference(const std::string& targetID);

	// Internal HDF5 writer
	bool WriteHDF5File(const std::string& filename,
		const std::string& xmlContent,
		const std::string& catalogueXML);
};


#endif

// From chatgpt
/*
*

//////
// structs

struct S421Waypoint {
	std::string id;
	std::string name;
	double lat;
	double lon;
};

struct S421Leg {
	std::string id;
	std::string fromID;
	std::string toID;
	std::string geometryType; // "loxodrome (= rhumb line)" or "orthodrome (= great circle)"
};

struct S421ScheduleElementRef {
	std::string id;
};

struct S421Schedule {
	std::string id;
	std::string name;
	std::string collectionID;
	std::vector<S421ScheduleElementRef> elements;
	enum Kind { Manual, Calculated, Recommended } kind;
};

struct S421RouteInfo {
	std::string id;
	std::string name;
	std::string author;
	std::string status; // must match routeInfoStatusType enumeration
	std::string vesselName;
	std::string vesselMMSI;
};

struct S421Route {
	std::string id;
	std::string routeID;
	int editionNo;
	std::vector<std::string> waypointIDs;
	std::vector<std::string> legIDs;
	std::string schedulesID;
};


/////

#include <pugixml.hpp>
#include <string>
#include <vector>

class S421RouteExporter {
public:
	S421RouteExporter() = default;

	pugi::xml_document Export(const S421RouteInfo& info,
		const S421Route& route,
		const std::vector<S421Waypoint>& waypoints,
		const std::vector<S421Leg>& legs,
		const std::vector<S421Schedule>& schedules) {
		pugi::xml_document doc;

		auto dataset = doc.append_child("Dataset");
		dataset.append_attribute("xmlns") = "http://www.iec.ch/S421/2.0";
		dataset.append_attribute("xmlns:gml") = "http://www.opengis.net/gml/3.2";
		dataset.append_attribute("xmlns:S100") = "http://www.iho.int/s100gml/5.0";

		auto members = dataset.append_child("members");

		AddRouteInfo(members, info);
		AddWaypoints(members, waypoints);
		AddLegs(members, legs);

		std::string routeSchedulesID = "RS1";
		AddRouteSchedulesContainer(members, routeSchedulesID, schedules);
		AddRouteSchedules(members, schedules);

		AddRoute(members, route, routeSchedulesID);

		return doc;
	}

private:
	static pugi::xml_node AddText(pugi::xml_node parent,
		const char* name,
		const std::string& value) {
		pugi::xml_node n = parent.append_child(name);
		n.append_child(pugi::node_pcdata).set_value(value.c_str());
		return n;
	}

	static pugi::xml_node AddHref(pugi::xml_node parent,
		const char* name,
		const std::string& targetID) {
		pugi::xml_node n = parent.append_child(name);
		n.append_attribute("href") = ("#" + targetID).c_str();
		return n;
	}

	static void AddGmlPoint(pugi::xml_node parent,
		const std::string& id,
		double lat, double lon) {
		auto pt = parent.append_child("gml:Point");
		pt.append_attribute("gml:id") = id.c_str();

		auto pos = pt.append_child("gml:pos");
		pos.append_child(pugi::node_pcdata)
			.set_value((std::to_string(lat) + " " + std::to_string(lon)).c_str());
	}

	// ---------------- RouteInfo ----------------
	void AddRouteInfo(pugi::xml_node members, const S421RouteInfo& info) {
		auto ri = members.append_child("RouteInfo");
		ri.append_attribute("id") = info.id.c_str();

		AddText(ri, "routeInfoName", info.name);
		AddText(ri, "routeInfoAuthor", info.author);
		AddText(ri, "routeInfoStatus", info.status);
		AddText(ri, "routeInfoVesselName", info.vesselName);
		AddText(ri, "routeInfoVesselMMSI", info.vesselMMSI);

		// Optional extensions block
		auto ext = ri.append_child("routeInfoExtensions");
		auto extBlock = ext.append_child("routeExtensions");
		AddText(extBlock, "routeExtensionsManufacturerId", "VendorX");
		AddText(extBlock, "routeExtensionsNote", "Generated by S421RouteExporter");
	}

	// ---------------- Waypoints ----------------
	void AddWaypoints(pugi::xml_node members,
		const std::vector<S421Waypoint>& waypoints) {
		for (const auto& wp : waypoints)
		{
			auto node = members.append_child("RouteWaypoint");
			node.append_attribute("id") = wp.id.c_str();

			AddText(node, "routeWaypointName", wp.name);

			auto geom = node.append_child("geometry");
			AddGmlPoint(geom, wp.id + "_pt", wp.lat, wp.lon);
		}
	}

	// ---------------- Legs ----------------
	void AddLegs(pugi::xml_node members,
		const std::vector<S421Leg>& legs) {
		for (const auto& lg : legs)
		{
			auto node = members.append_child("RouteWaypointLeg");
			node.append_attribute("id") = lg.id.c_str();

			AddHref(node, "routeWaypointLegStart", lg.fromID);
			AddHref(node, "routeWaypointLegEnd", lg.toID);

			AddText(node, "routeWaypointLegGeometryType",
				lg.geometryType.empty()
				? "loxodrome (= rhumb line)"
				: lg.geometryType);
		}
	}

	// ---------------- RouteSchedules container ----------------
	void AddRouteSchedulesContainer(pugi::xml_node members,
		const std::string& id,
		const std::vector<S421Schedule>& schedules) {
		auto rs = members.append_child("RouteSchedules");
		rs.append_attribute("id") = id.c_str();

		rs.append_child("routeSchedulesExtensions");

		for (const auto& s : schedules)
			AddHref(rs, "routeSchedule", s.id);
	}

	// ---------------- RouteSchedule + variants ----------------
	void AddRouteSchedules(pugi::xml_node members,
		const std::vector<S421Schedule>& schedules) {
		for (const auto& s : schedules)
		{
			auto sch = members.append_child("RouteSchedule");
			sch.append_attribute("id") = s.id.c_str();

			AddText(sch, "routeScheduleID", "1");
			AddText(sch, "routeScheduleName", s.name);
			sch.append_child("routeScheduleExtensions");
			AddHref(sch, "routeScheduleCollection", s.collectionID);

			switch (s.kind)
			{
			case S421Schedule::Manual:
				AddHref(sch, "routeScheduleManual", s.id + "_MAN");
				AddScheduleManual(members, s, s.id + "_MAN");
				break;
			case S421Schedule::Calculated:
				AddHref(sch, "routeScheduleCalculated", s.id + "_CALC");
				AddScheduleCalculated(members, s, s.id + "_CALC");
				break;
			case S421Schedule::Recommended:
				AddHref(sch, "routeScheduleRecommended", s.id + "_REC");
				AddScheduleRecommended(members, s, s.id + "_REC");
				break;
			}
		}
	}

	void AddScheduleManual(pugi::xml_node members,
		const S421Schedule& s,
		const std::string& id) {
		auto sm = members.append_child("RouteScheduleManual");
		sm.append_attribute("id") = id.c_str();

		sm.append_child("routeScheduleManualExtensions");
		AddHref(sm, "routeScheduleCollection", s.collectionID);

		for (const auto& e : s.elements)
			AddHref(sm, "routeScheduleElement", e.id);
	}

	void AddScheduleCalculated(pugi::xml_node members,
		const S421Schedule& s,
		const std::string& id) {
		auto sc = members.append_child("RouteScheduleCalculated");
		sc.append_attribute("id") = id.c_str();

		sc.append_child("routeScheduleCalculatedExtensions");
		AddHref(sc, "routeScheduleCollection", s.collectionID);

		for (const auto& e : s.elements)
			AddHref(sc, "routeScheduleElement", e.id);
	}

	void AddScheduleRecommended(pugi::xml_node members,
		const S421Schedule& s,
		const std::string& id) {
		auto sr = members.append_child("RouteScheduleRecommended");
		sr.append_attribute("id") = id.c_str();

		sr.append_child("routeScheduleRecommendedExtensions");
		AddHref(sr, "routeScheduleCollection", s.collectionID);

		for (const auto& e : s.elements)
			AddHref(sr, "routeScheduleElement", e.id);
	}

	// ---------------- Route ----------------
	void AddRoute(pugi::xml_node members,
		const S421Route& r,
		const std::string& schedulesID) {
		auto route = members.append_child("Route");
		route.append_attribute("id") = r.id.c_str();

		AddText(route, "routeFormatVersion", "2.0.0");
		AddText(route, "routeID", r.routeID);
		AddText(route, "routeEditionNo", std::to_string(r.editionNo));

		for (const auto& wpID : r.waypointIDs)
			AddHref(route, "routeWaypoint", wpID);

		for (const auto& lgID : r.legIDs)
			AddHref(route, "routeWaypointLeg", lgID);

		AddHref(route, "routeSchedules", schedulesID);
	}
};

	doc.save_file("s421_output.xml", "  "); // pretty-print with indentation
}

// usage
int main() {
	S421RouteInfo info{
		"RI1", "Isla de Arosa – Test Route", "Steven",
		"Planned", "MyShip", "123456789"
	};

	std::vector<S421Waypoint> wps{
		{"WP1", "Departure", 42.56123, -8.86512},
		{"WP2", "Midpoint", 42.60000, -8.90000},
		{"WP3", "Arrival", 42.65000, -8.95000}
	};

	std::vector<S421Leg> legs{
		{"LEG1", "WP1", "WP2", "loxodrome (= rhumb line)"},
		{"LEG2", "WP2", "WP3", "loxodrome (= rhumb line)"}
	};

	std::vector<S421Schedule> schedules{
		{"SCH1", "Manual Schedule", "COL1",
		 {{"EL1"}, {"EL2"}}, S421Schedule::Manual},
		{"SCH2", "Calculated Schedule", "COL1",
		 {{"EL3"}, {"EL4"}}, S421Schedule::Calculated},
		{"SCH3", "Recommended Schedule", "COL1",
		 {{"EL5"}, {"EL6"}}, S421Schedule::Recommended}
	};

	S421Route route{
		"ROUTE_MAIN", "ROUTE001", 1,
		{"WP1", "WP2", "WP3"},
		{"LEG1", "LEG2"},
		"RS1"
	};

	S421RouteExporter exporter;
	auto doc = exporter.Export(info, route, wps, legs, schedules);
	doc.save_file("s421_complete.xml", "  ");

	return 0;
}
*/