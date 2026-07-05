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
// Description: Derived from Demo plugin, demonstrate export of waypoints formatted using S421
// Owner: twocanplugin@hotmail.com
// Date: 31/03/2026
// Version History: 
// 1.0 Initial Release
// 03/05/2026 - 1.1 Export waypoints using GeoJSON format
// 14/05/2026 - 1.2 Export Waypoints to SignalK
// 30/06/2026 - 1.3 Export Routes using RTZ and IHO S-421

#include "demo_s421.h"
#include <H5Cpp.h>

#include <sstream>

DemoS421::DemoS421() {
}

std::string DemoS421::MakeFeatureID(const std::string& prefix) {
    return prefix + "_" + std::to_string(nextFeatureId++);
}

void DemoS421::CreateDataset(const std::string& datasetIdentifier) {
    doc.reset();

    // Root S-100 Dataset
    pugi::xml_node root = doc.append_child("S100:Dataset");
    root.append_attribute("gml:id") = datasetIdentifier.c_str();

    // Feature container
    featureContainer = root.append_child("S100:FeatureContainer");
}

void DemoS421::AddRouteInfo(const std::string& name,
    const std::string& author,
    const std::string& status) {
    std::string id = MakeFeatureID("RouteInfo");

    pugi::xml_node info = featureContainer.append_child("RouteInfo");
    info.append_attribute("gml:id") = id.c_str();

    info.append_child("routeInfoName").text() = name.c_str();
    if (!author.empty())
        info.append_child("routeInfoAuthor").text() = author.c_str();
    info.append_child("routeInfoStatus").text() = status.c_str();
}

pugi::xml_node DemoS421::AddGMLPoint(double lat, double lon) {
    std::string id = MakeFeatureID("Point");

    pugi::xml_node point = featureContainer.append_child("gml:Point");
    point.append_attribute("gml:id") = id.c_str();

    pugi::xml_node pos = point.append_child("gml:pos");
    pos.text() = (std::to_string(lat) + " " + std::to_string(lon)).c_str();

    return point;
}

pugi::xml_node DemoS421::AddReference(const std::string& targetID) {
    pugi::xml_node ref = featureContainer.append_child("gml:ReferenceType");
    ref.append_attribute("xlink:href") = ("#" + targetID).c_str();
    return ref;
}

void DemoS421::AddWaypoint(const S421Waypoint& wp) {
    std::string id = MakeFeatureID("RouteWaypoint");

    pugi::xml_node w = featureContainer.append_child("RouteWaypoint");
    w.append_attribute("gml:id") = id.c_str();

    w.append_child("routeWaypointID").text() = wp.id;
    if (!wp.name.empty())
        w.append_child("routeWaypointName").text() = wp.name.c_str();

    // Geometry: reference to a gml:Point
    pugi::xml_node geom = w.append_child("routeWaypointGeometry");
    pugi::xml_node point = AddGMLPoint(wp.lat, wp.lon);
    geom.append_attribute("xlink:href") =
        ("#" + std::string(point.attribute("gml:id").value())).c_str();
}

void DemoS421::AddSchedule(const S421Schedule& schedule) {
    // RouteSchedule feature
    std::string schedId = MakeFeatureID("RouteSchedule");
    pugi::xml_node s = featureContainer.append_child("RouteSchedule");
    s.append_attribute("gml:id") = schedId.c_str();

    s.append_child("routeScheduleID").text() = schedule.id;
    if (!schedule.name.empty())
        s.append_child("routeScheduleName").text() = schedule.name.c_str();

    // Manual schedule
    if (!schedule.manual.empty())
    {
        std::string manualId = MakeFeatureID("RouteScheduleManual");
        pugi::xml_node manual = featureContainer.append_child("RouteScheduleManual");
        manual.append_attribute("gml:id") = manualId.c_str();

        // Collection reference (minimal)
        pugi::xml_node coll = manual.append_child("routeScheduleCollection");
        coll.append_attribute("xlink:href") = ("#" + manualId + "_COLL").c_str();

        for (const auto& se : schedule.manual)
        {
            std::string elemId = MakeFeatureID("RouteScheduleElement");
            pugi::xml_node elem = featureContainer.append_child("RouteScheduleElement");
            elem.append_attribute("gml:id") = elemId.c_str();

            elem.append_child("routeScheduleElementWaypointID").text() = se.waypointId;
            if (!se.etd.empty())
                elem.append_child("routeScheduleElementETD").text() = se.etd.c_str();
            if (!se.eta.empty())
                elem.append_child("routeScheduleElementETA").text() = se.eta.c_str();
            elem.append_child("routeScheduleElementSpeed").text() = se.speed;

            pugi::xml_node ref = manual.append_child("routeScheduleElement");
            ref.append_attribute("xlink:href") = ("#" + elemId).c_str();
        }

        pugi::xml_node mref = s.append_child("routeScheduleManual");
        mref.append_attribute("xlink:href") = ("#" + manualId).c_str();
    }

    // Calculated schedule
    if (!schedule.calculated.empty())
    {
        std::string calcId = MakeFeatureID("RouteScheduleCalculated");
        pugi::xml_node calc = featureContainer.append_child("RouteScheduleCalculated");
        calc.append_attribute("gml:id") = calcId.c_str();

        pugi::xml_node coll = calc.append_child("routeScheduleCollection");
        coll.append_attribute("xlink:href") = ("#" + calcId + "_COLL").c_str();

        for (const auto& se : schedule.calculated)
        {
            std::string elemId = MakeFeatureID("RouteScheduleElement");
            pugi::xml_node elem = featureContainer.append_child("RouteScheduleElement");
            elem.append_attribute("gml:id") = elemId.c_str();

            elem.append_child("routeScheduleElementWaypointID").text() = se.waypointId;
            if (!se.etd.empty())
                elem.append_child("routeScheduleElementETD").text() = se.etd.c_str();
            if (!se.eta.empty())
                elem.append_child("routeScheduleElementETA").text() = se.eta.c_str();
            elem.append_child("routeScheduleElementSpeed").text() = se.speed;

            pugi::xml_node ref = calc.append_child("routeScheduleElement");
            ref.append_attribute("xlink:href") = ("#" + elemId).c_str();
        }

        pugi::xml_node cref = s.append_child("routeScheduleCalculated");
        cref.append_attribute("xlink:href") = ("#" + calcId).c_str();
    }
}

bool DemoS421::SaveXML(const std::string& filename) {
    return doc.save_file(filename.c_str(), "  ");
}


// ------------------------------------------------------------
// Add S-100 Metadata Blocks
// ------------------------------------------------------------
void DemoS421::AddS100Metadata(const S100Metadata& meta) {
    pugi::xml_node root = doc.child("S100:Dataset");

    // -------------------------------
    // DatasetDiscoveryMetadata
    // -------------------------------
    pugi::xml_node dmd = root.append_child("S100:DatasetDiscoveryMetadata");

    dmd.append_child("S100:productSpecification").text() =
        meta.productSpecification.c_str();
    dmd.append_child("S100:issueDate").text() =
        meta.issueDate.c_str();
    dmd.append_child("S100:datasetID").text() =
        root.attribute("gml:id").value();
    dmd.append_child("S100:producingAgency").text() =
        meta.producingAgency.c_str();

    // Bounding box
    pugi::xml_node bbox = dmd.append_child("S100:boundingBox");
    pugi::xml_node env = bbox.append_child("gml:Envelope");

    env.append_child("gml:lowerCorner").text() =
        (std::to_string(meta.minLat) + " " +
            std::to_string(meta.minLon)).c_str();

    env.append_child("gml:upperCorner").text() =
        (std::to_string(meta.maxLat) + " " +
            std::to_string(meta.maxLon)).c_str();

    // -------------------------------
    // DataCoverage
    // -------------------------------
    pugi::xml_node cov = root.append_child("S100:DataCoverage");
    pugi::xml_node se = cov.append_child("S100:spatialExtent");
    pugi::xml_node env2 = se.append_child("gml:Envelope");

    env2.append_child("gml:lowerCorner").text() =
        (std::to_string(meta.minLat) + " " +
            std::to_string(meta.minLon)).c_str();

    env2.append_child("gml:upperCorner").text() =
        (std::to_string(meta.maxLat) + " " +
            std::to_string(meta.maxLon)).c_str();
}

// ------------------------------------------------------------
// Generate minimal S-100 ExchangeCatalogue XML
// ------------------------------------------------------------
static std::string BuildExchangeCatalogueXML(const std::string& datasetID,
    const std::string& productSpec) {
    pugi::xml_document cat;

    pugi::xml_node root = cat.append_child("S100:ExchangeCatalogue");

    pugi::xml_node ds = root.append_child("S100:dataset");
    ds.append_child("S100:datasetID").text() = datasetID.c_str();
    ds.append_child("S100:productSpecification").text() =
        productSpec.c_str();

    std::stringstream ss;
    cat.save(ss, "  ");
    return ss.str();
}

// ------------------------------------------------------------
// Internal HDF5 Writer
// ------------------------------------------------------------
bool DemoS421::WriteHDF5File(const std::string& filename,
    const std::string& xmlContent,
    const std::string& catalogueXML) {
    try
    {
        H5::H5File file(filename, H5F_ACC_TRUNC);

        // Root folder
        H5::Group root = file.createGroup("/S100_ROOT");

        // DataSets/S421
        H5::Group datasets = file.createGroup("/S100_ROOT/DataSets");
        H5::Group s421 = datasets.createGroup("/S100_ROOT/DataSets/S421");

        // Write S421.xml
        {
            H5::DataSpace space(H5S_SCALAR);
            H5::StrType strType(H5::PredType::C_S1, xmlContent.size());
            H5::DataSet ds = s421.createDataSet("S421.xml", strType, space);
            ds.write(xmlContent.c_str(), strType);
        }

        // ExchangeCatalogue
        H5::Group ec = file.createGroup("/S100_ROOT/ExchangeCatalogue");

        {
            H5::DataSpace space(H5S_SCALAR);
            H5::StrType strType(H5::PredType::C_S1, catalogueXML.size());
            H5::DataSet ds = ec.createDataSet("catalogue.xml", strType, space);
            ds.write(catalogueXML.c_str(), strType);
        }

        return true;
    }
    catch (const H5::Exception&)
    {
        return false;
    }
}

// ------------------------------------------------------------
// Save HDF5 (GDAL-compatible)
// ------------------------------------------------------------
bool DemoS421::SaveHDF5(const std::string& filename) {
    // Convert XML document to string
    std::stringstream ss;
    doc.save(ss, "  ");
    std::string xmlContent = ss.str();

    // Build ExchangeCatalogue
    std::string datasetID = doc.child("S100:Dataset").attribute("gml:id").value();
    std::string productSpec =
        doc.child("S100:Dataset")
        .child("S100:DatasetDiscoveryMetadata")
        .child("S100:productSpecification").text().as_string();

    std::string catalogueXML =
        BuildExchangeCatalogueXML(datasetID, productSpec);

    // Write HDF5 container
    return WriteHDF5File(filename, xmlContent, catalogueXML);
}


