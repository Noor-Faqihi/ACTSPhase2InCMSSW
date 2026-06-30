/// ######################################################################################
/// # Plugin used to convert the CMSSW Phase-2 detector elements into ACTS detector     #
/// # elements and build the ACTS TrackingGeometry for the CMS Phase-2 tracker.         #
/// #                                                                                   #
/// # Subdetectors covered:                                                             #
/// #   Inner Tracker (IT):                                                             #
/// #     - TBPX  : 4 barrel layers  (PixelSubdetector::PixelBarrel, subdetId=1)       #
/// #     - TFPX  : 12 forward discs (PixelSubdetector::PixelEndcap, subdetId=2,       #
/// #               disk <= 12)                                                         #
/// #     - TEPX  : 4 extended discs (PixelSubdetector::PixelEndcap, subdetId=2,       #
/// #               disk > 12)                                                          #
/// #   Outer Tracker (OT):                                                             #
/// #     - T2B   : 6 barrel layers  (subdetId=4, PS + 2S stacked modules)             #
/// #     - T2E   : 5 endcap discs   (subdetId=5, PS inner rings + 2S outer rings)     #
/// #                                                                                   #
/// #                                                                                   #
/// # Adapted from ACTSTrackingGeometryProducer.cc (Phase-1) by ldamenti.              #
/// ######################################################################################

// ===== CMSSW Framework =====
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Utilities/interface/typelookup.h"
#include "FWCore/Framework/interface/ESConsumesCollector.h"

// ===== Geometry / Topology =====
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"  
#include "Geometry/Records/interface/ACTSTrackerGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"

// ===== Alignment =====
#include "CondFormats/AlignmentRecord/interface/TrackerAlignmentRcd.h"
#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/Alignment/interface/AlignTransform.h"
#include "Alignment/CommonAlignment/interface/Alignable.h"

// ===== Detector surface bounds =====
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

// ===== Phase-2 Detector IDs =====
// Inner Tracker pixel subdetector IDs (same as Phase-1 pixel)
#include "DataFormats/TrackerCommon/interface/PixelBarrelName.h"
#include "DataFormats/TrackerCommon/interface/PixelEndcapName.h"
// Phase-2 Outer Tracker: subdetId 4 (T2B) and 5 (T2E) — no dedicated StripSubdetector header needed
#include "DataFormats/DetId/interface/DetId.h"

// ===== Math helpers =====
#include "DetectorDescription/Core/interface/DDRotationMatrix.h"
#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "Math/RotationZ.h"
#include "Math/Rotation3D.h"
#include "Math/AxisAngle.h"

// ===== ACTS core =====
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/TrackingGeometryVisitor.hpp"
#include "Acts/Geometry/Polyhedron.hpp"
#include "Acts/Geometry/Blueprint.hpp"
#include "Acts/Geometry/ContainerBlueprintNode.hpp"
#include "Acts/Geometry/LayerBlueprintNode.hpp"
#include "Acts/Geometry/MaterialDesignatorBlueprintNode.hpp"
#include "Acts/Geometry/SurfaceArrayCreator.hpp"

// ===== ACTS detector / surfaces =====
#include "Acts/Detector/KdtSurfacesProvider.hpp"
#include "Acts/Detector/ProtoDetector.hpp"
#include "Acts/Detector/detail/ReferenceGenerators.hpp"
#include "Acts/Detector/interface/ISurfacesProvider.hpp"
#include "Acts/Detector/LayerStructureBuilder.hpp"
#include "Acts/Detector/detail/BlueprintHelper.hpp"
#include "Acts/Detector/detail/BlueprintDrawer.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/TrapezoidBounds.hpp"
#include "Acts/Navigation/SurfaceArrayNavigationPolicy.hpp"
#include "Acts/Navigation/TryAllNavigationPolicy.hpp"
//#include "ActsPlugins/TGeo/TGeoDetectorElement.hpp" 

// ===== ACTS visualization =====
#include "Acts/Visualization/GeometryView3D.hpp"
#include "Acts/Visualization/ObjVisualization3D.hpp"
#include "Acts/Visualization/ViewConfig.hpp"
#include "ActsPlugins/ActSVG/DetectorSvgConverter.hpp"
#include "ActsPlugins/ActSVG/TrackingGeometrySvgConverter.hpp"
#include "ActsPlugins/ActSVG/SurfaceArraySvgConverter.hpp"

// ===== ACTS material =====
#include "Acts/Material/PropagatorMaterialAssigner.hpp"
#include "Acts/Material/BinnedSurfaceMaterialAccumulater.hpp"
#include "Acts/Material/BinnedSurfaceMaterial.hpp"
#include "Acts/Material/AccumulatedMaterialSlab.hpp"
#include "Acts/Material/HomogeneousSurfaceMaterial.hpp"
#include "Acts/Material/IntersectionMaterialAssigner.hpp"
#include "Acts/Material/MaterialValidater.hpp"
#include "Acts/Material/MaterialMapper.hpp"
#include "ActsPlugins/Json/JsonDetectorElement.hpp"
#include "ActsPlugins/Json/JsonSurfacesReader.hpp"
#include "ActsPlugins/Json/JsonMaterialDecorator.hpp"
#include "ActsPlugins/Json/SurfaceJsonConverter.hpp"
#include "ActsPlugins/Root/RootMaterialTrackIo.hpp"

// ===== ROOT / nlohmann =====
#include "TChain.h"
#include "TFile.h"
#include <nlohmann/json.hpp>

// ===== STL =====
#include <fstream>
#include <iomanip>
#include <random>
#include <iostream>
#include <string>
#include <vector>
#include <numbers>

// ===== Local =====
#include "ACTSPhase2InCMSSW/ACTSPhase2InCMSSW/interface/CMSDetectorElement.hpp" 
#include "ACTSPhase2InCMSSW/ACTSPhase2InCMSSW/interface/JsonMaterialWriter.hpp"

// ==================================================================================
// Type aliases
// ==================================================================================
using json = nlohmann::json;
using KdtSurfacesDim2Bin100 = Acts::Experimental::KdtSurfaces<2u, 100u>;

// KDT casts: (AxisZ, AxisR) — query by {z_min,r_min} -> {z_max,r_max}
const std::array<Acts::AxisDirection, 2UL> casts{Acts::AxisDirection::AxisZ,
                                                  Acts::AxisDirection::AxisR};
using DetElVect = std::vector<std::shared_ptr<Acts::CMSDetectorElement>>;
TYPELOOKUP_DATA_REG(Alignments);

// ==================================================================================
// Phase-2 Outer Tracker subdetector IDs
// NOTE: In Phase-2 CMSSW the OT reuses the numeric subdet slots previously held
// by TIB/TOB/TEC/TID. Verify against your geometry tag's TrackerTopology.
// ==================================================================================
namespace Phase2OT {
  constexpr int Barrel  = 4; // T2B — 6 layers of PS/2S stacked modules
  constexpr int Endcap  = 5; // T2E — 5 discs per side (±Z)
}

// ==================================================================================
// Helper: inner/outer classification for stacked modules (Phase-1 logic, reused)
// Uses the sign of (pos · normal) and TrackerTopology::isLower() to determine
// which sensor of a PS/2S stack is the "inner" (closer to beam) one.
// ==================================================================================
//This is for debugging purposes 
int innerOuterFromOrientation(const TrackerTopology& tTopo,
                               DetId detId,
                               const GlobalPoint& pos,
                               const GlobalVector& nrm) {
  const double dot = pos.x() * nrm.x() + pos.y() * nrm.y() + pos.z() * nrm.z();
  const bool normalOut = (dot > 0.0);
  const bool isLower   = tTopo.isLower(detId);
  return normalOut ? (isLower ? 0 : 1) : (isLower ? 1 : 0); //Whats that 
}

// ==================================================================================
// Output data structure returned by the ESProducer
// ==================================================================================
struct TrackingGeometryWithDetEls {
  DetElVect detElements;
  std::shared_ptr<Acts::TrackingGeometry> trackingGeometry;
};
TYPELOOKUP_DATA_REG(TrackingGeometryWithDetEls);

// ==================================================================================
//  MATERIAL INFRASTRUCTURE  (identical to Phase-1 version)
// ==================================================================================

struct MaterialConfig {
  std::string outputMaterialTracks       = "material-tracks";
  std::string treeName                   = "material-tracks";
  std::vector<std::string> fileList;
  bool readCachedSurfaceInformation      = false;
};

template <typename element_t, typename index_t>
void stableSort(index_t numElements, const element_t* elements,
                index_t* sortedIndices, Bool_t sortDescending) {
  for (index_t i = 0; i < numElements; i++) sortedIndices[i] = i;
  if (sortDescending)
    std::stable_sort(sortedIndices, sortedIndices + numElements,
                     CompareDesc<const element_t*>(elements));
  else
    std::stable_sort(sortedIndices, sortedIndices + numElements,
                     CompareAsc<const element_t*>(elements));
}

class MyMaterialEvReader {
public:
  explicit MyMaterialEvReader(const MaterialConfig& config) : m_cfg(config) {
    ActsPlugins::RootMaterialTrackIo::Config ioCfg;
    m_payload     = std::make_unique<ActsPlugins::RootMaterialTrackIo>(ioCfg);
    m_inputChain  = std::make_unique<TChain>(m_cfg.treeName.c_str());
    for (const auto& f : m_cfg.fileList) m_inputChain->Add(f.c_str());
    m_payload->connectForRead(*m_inputChain);
    std::size_t nentries = m_inputChain->GetEntries();
    m_events    = static_cast<std::size_t>(m_inputChain->GetMaximum("event_id") + 1);
    m_batchSize = nentries / m_events;
    m_inputChain->SetEstimate(nentries + 1);
    m_entryNumbers.resize(nentries);
    m_inputChain->Draw("event_id", "", "goff");
    stableSort(m_inputChain->GetEntries(), m_inputChain->GetV1(),
               m_entryNumbers.data(), false);
  }

  std::unordered_map<std::size_t, Acts::RecordedMaterialTrack> read(std::size_t evNum) {
    mtrackCollection.clear();
    for (std::size_t ib = 0; ib < m_batchSize; ++ib) {
      auto entry = m_entryNumbers.at(m_batchSize * evNum + ib);
      m_inputChain->GetEntry(entry);
      mtrackCollection[ib] = m_payload->read();
    }
    return mtrackCollection;
  }

private:
  MaterialConfig m_cfg;
  std::unique_ptr<TChain> m_inputChain;
  std::unique_ptr<ActsPlugins::RootMaterialTrackIo> m_payload;
  std::vector<long long> m_entryNumbers;
  std::size_t m_events    = 0;
  std::size_t m_batchSize = 0;
  std::unordered_map<std::size_t, Acts::RecordedMaterialTrack> mtrackCollection;
};

struct myContext {
  Acts::GeometryContext    geoContext;
  Acts::MagneticFieldContext magFieldContext;
  std::size_t EvNumber;
};

// ==================================================================================
// Visualization helper
// ==================================================================================
void writeSurfacesObj(const std::vector<std::shared_ptr<Acts::Surface>>& surfaces,
                      const std::string& fileName) {
  Acts::ObjVisualization3D obj;
  for (size_t i = 0; i < surfaces.size(); ++i) {
    if (!surfaces[i]) { std::cerr << "[ERROR] Surface " << i << " is nullptr!\n"; continue; }
    Acts::GeometryView3D::drawSurface(obj, *surfaces[i], Acts::GeometryContext{},
                                      Acts::Transform3::Identity(), Acts::ViewConfig{});
  }
  obj.write(fileName);
  obj.clear();
}

Acts::Transform3 GenerateTranslation(double dx, double dy, double dz) {
  return Acts::Transform3::Identity() * Acts::Translation3{Acts::Vector3{dx, dy, dz}};
}

// ==================================================================================
// SelectActiveSurfaces_Phase2
//
// Selects surfaces from the KDT tree by (Z, R) bounding box for each
// named Phase-2 layer / disc.
//
// Naming convention:
//   IT barrel  : "TBPX_L0" ... "TBPX_L3"
//   IT fwd     : "TFPX_Pos0" ... "TFPX_Pos11"  (and Neg)
//   IT ext     : "TEPX_Pos0" ... "TEPX_Pos3"    (and Neg)
//   OT barrel  : "T2B_L0"  ... "T2B_L5"
//   OT endcap  : "T2E_Pos0" ... "T2E_Pos4"      (and Neg)
//
// All coordinates are in mm (ACTS units).
// NOTE: Boundary values are derived from the Phase-2 TDR (CERN-LHCC-2017-009)
// and should be verified against your specific geometry tag.
// ==================================================================================
std::vector<std::shared_ptr<Acts::Surface>>
SelectActiveSurfaces_Phase2(const KdtSurfacesDim2Bin100& surfaces,
                             const std::string& Layer_name) {

  Acts::RangeXD<2, double, std::array> Range({0, 0}, {0, 0});

  // ----- Inner Tracker: TBPX (4 barrel layers) -----
  // R values (mm): 24, 37, 52, 68 — Z half-length ~270 mm
  if      (Layer_name == "TBPX_L0") Range = {{-275,  19}, { 275,  29}};
  else if (Layer_name == "TBPX_L1") Range = {{-275,  32}, { 275,  43}};
  else if (Layer_name == "TBPX_L2") Range = {{-275,  47}, { 275,  58}};
  else if (Layer_name == "TBPX_L3") Range = {{-275,  63}, { 275,  75}};

  // ----- Inner Tracker: TFPX positive side (12 discs, Z ~ 291..1084 mm) -----
  // R range: ~40..256 mm for each disc
  else if (Layer_name == "TFPX_Pos0")  Range = {{ 280,  35}, { 305, 265}};
  else if (Layer_name == "TFPX_Pos1")  Range = {{ 385,  35}, { 415, 265}};
  else if (Layer_name == "TFPX_Pos2")  Range = {{ 490,  35}, { 520, 265}};
  else if (Layer_name == "TFPX_Pos3")  Range = {{ 605,  35}, { 635, 265}};
  else if (Layer_name == "TFPX_Pos4")  Range = {{ 720,  35}, { 750, 265}};
  else if (Layer_name == "TFPX_Pos5")  Range = {{ 840,  35}, { 875, 265}};
  else if (Layer_name == "TFPX_Pos6")  Range = {{ 965,  35}, { 998, 265}};
  else if (Layer_name == "TFPX_Pos7")  Range = {{1085,  35}, {1115, 265}};
  else if (Layer_name == "TFPX_Pos8")  Range = {{1195,  35}, {1225, 265}};
  else if (Layer_name == "TFPX_Pos9")  Range = {{1275,  35}, {1305, 265}};
  else if (Layer_name == "TFPX_Pos10") Range = {{1355,  35}, {1390, 265}};
  else if (Layer_name == "TFPX_Pos11") Range = {{1435,  35}, {1470, 265}};

  // ----- Inner Tracker: TFPX negative side -----
  else if (Layer_name == "TFPX_Neg0")  Range = {{-305,  35}, {-280, 265}};
  else if (Layer_name == "TFPX_Neg1")  Range = {{-415,  35}, {-385, 265}};
  else if (Layer_name == "TFPX_Neg2")  Range = {{-520,  35}, {-490, 265}};
  else if (Layer_name == "TFPX_Neg3")  Range = {{-635,  35}, {-605, 265}};
  else if (Layer_name == "TFPX_Neg4")  Range = {{-750,  35}, {-720, 265}};
  else if (Layer_name == "TFPX_Neg5")  Range = {{-875,  35}, {-840, 265}};
  else if (Layer_name == "TFPX_Neg6")  Range = {{-998,  35}, {-965, 265}};
  else if (Layer_name == "TFPX_Neg7")  Range = {{-1115, 35}, {-1085,265}};
  else if (Layer_name == "TFPX_Neg8")  Range = {{-1225, 35}, {-1195,265}};
  else if (Layer_name == "TFPX_Neg9")  Range = {{-1305, 35}, {-1275,265}};
  else if (Layer_name == "TFPX_Neg10") Range = {{-1390, 35}, {-1355,265}};
  else if (Layer_name == "TFPX_Neg11") Range = {{-1470, 35}, {-1435,265}};

  // ----- Inner Tracker: TEPX positive side (4 discs, Z ~ 1665..1965 mm) -----
  // R range: ~65..270 mm
  else if (Layer_name == "TEPX_Pos0") Range = {{1655,  60}, {1685, 275}};
  else if (Layer_name == "TEPX_Pos1") Range = {{1770,  60}, {1800, 275}};
  else if (Layer_name == "TEPX_Pos2") Range = {{1885,  60}, {1915, 275}};
  else if (Layer_name == "TEPX_Pos3") Range = {{1950,  60}, {1980, 275}};

  // ----- Inner Tracker: TEPX negative side -----
  else if (Layer_name == "TEPX_Neg0") Range = {{-1685, 60}, {-1655,275}};
  else if (Layer_name == "TEPX_Neg1") Range = {{-1800, 60}, {-1770,275}};
  else if (Layer_name == "TEPX_Neg2") Range = {{-1915, 60}, {-1885,275}};
  else if (Layer_name == "TEPX_Neg3") Range = {{-1980, 60}, {-1950,275}};

  // ----- Outer Tracker: T2B barrel (6 layers) -----
  // R values (mm): ~230, 299, 371, 443, 516, 590  |Z| < ~1200 mm
  else if (Layer_name == "T2B_L0") Range = {{-1215, 220}, {1215, 245}};
  else if (Layer_name == "T2B_L1") Range = {{-1215, 288}, {1215, 313}};
  else if (Layer_name == "T2B_L2") Range = {{-1215, 360}, {1215, 385}};
  else if (Layer_name == "T2B_L3") Range = {{-1215, 432}, {1215, 457}};
  else if (Layer_name == "T2B_L4") Range = {{-1215, 505}, {1215, 530}};
  else if (Layer_name == "T2B_L5") Range = {{-1215, 579}, {1215, 605}};

  // ----- Outer Tracker: T2E endcap positive side (5 discs) -----
  // Z ~ +1320, +1570, +1840, +2110, +2380 mm  R: 230..1050 mm
  else if (Layer_name == "T2E_Pos0") Range = {{1305, 220}, {1340, 1060}};
  else if (Layer_name == "T2E_Pos1") Range = {{1555, 220}, {1590, 1060}};
  else if (Layer_name == "T2E_Pos2") Range = {{1825, 220}, {1860, 1060}};
  else if (Layer_name == "T2E_Pos3") Range = {{2095, 220}, {2130, 1060}};
  else if (Layer_name == "T2E_Pos4") Range = {{2360, 220}, {2400, 1060}};

  // ----- Outer Tracker: T2E endcap negative side -----
  else if (Layer_name == "T2E_Neg0") Range = {{-1340, 220}, {-1305,1060}};
  else if (Layer_name == "T2E_Neg1") Range = {{-1590, 220}, {-1555,1060}};
  else if (Layer_name == "T2E_Neg2") Range = {{-1860, 220}, {-1825,1060}};
  else if (Layer_name == "T2E_Neg3") Range = {{-2130, 220}, {-2095,1060}};
  else if (Layer_name == "T2E_Neg4") Range = {{-2400, 220}, {-2360,1060}};

  return surfaces.surfaces(Range);
}

// ==================================================================================
// Navigation policy (binning) — identical to Phase-1
// ==================================================================================
void makeBinning(auto& layer,
                 Acts::SurfaceArrayNavigationPolicy::LayerType layer_type,
                 double Bin0, double Bin1) {
  layer.setNavigationPolicyFactory(
    Acts::NavigationPolicyFactory()
      .add<Acts::SurfaceArrayNavigationPolicy>(
        Acts::SurfaceArrayNavigationPolicy::Config{.layerType = layer_type,
                                                   .bins = {Bin0, Bin1}})
      .add<Acts::TryAllNavigationPolicy>(
        Acts::TryAllNavigationPolicy::Config{.sensitives = true})
      .asUniquePtr());
}

// ==================================================================================
// Blueprint helpers — add gap / static volume with material faces
// ==================================================================================
void AddExtraLayer(const std::string& gapName,
                   bool isBarrel,
                   Acts::Experimental::CylinderContainerBlueprintNode* cont,
                   Acts::Transform3 transform,
                   std::shared_ptr<Acts::CylinderVolumeBounds> bounds) {
  Acts::Transform3 base{Acts::Transform3::Identity()};
  Acts::AxisDirection dir = isBarrel ? Acts::AxisDirection::AxisR
                                     : Acts::AxisDirection::AxisZ;

  cont->addCylinderContainer(gapName.c_str(), dir, [&](auto& gap) {
    gap.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
       .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
    gap.addMaterial(gapName.c_str(),
      [&](Acts::Experimental::MaterialDesignatorBlueprintNode& mat) {
        if (!isBarrel) {
          mat.configureFace(Acts::CylinderVolumeBounds::Face::NegativeDisc,
                            {Acts::AxisDirection::AxisR,   Acts::AxisBoundaryType::Bound, 100},
                            {Acts::AxisDirection::AxisPhi, Acts::AxisBoundaryType::Bound, 100});
          mat.configureFace(Acts::CylinderVolumeBounds::Face::PositiveDisc,
                            {Acts::AxisDirection::AxisR,   Acts::AxisBoundaryType::Bound, 100},
                            {Acts::AxisDirection::AxisPhi, Acts::AxisBoundaryType::Bound, 100});
        } else {
          mat.configureFace(Acts::CylinderVolumeBounds::Face::OuterCylinder,
                            {Acts::AxisDirection::AxisRPhi, Acts::AxisBoundaryType::Bound, 100},
                            {Acts::AxisDirection::AxisZ,    Acts::AxisBoundaryType::Bound, 100});
        }
        mat.addCylinderContainer(gapName.c_str(), Acts::AxisDirection::AxisZ,
          [&](auto& L) { L.addStaticVolume(base * transform, bounds, gapName.c_str()); });
      });
  });
}

// ==================================================================================
// AddDiskLayer_and_Material
// Wraps a disc layer (IT forward/extended or OT endcap) in a material designator.
// ==================================================================================
template <typename MakeLayerFn>
void AddDiskLayer_and_Material(Acts::Experimental::CylinderContainerBlueprintNode* cont,
                                const std::string& LayerName,
                                MakeLayerFn&& makeLayerFunc,
                                double z_pos,
                                double bin0, double bin1) {
  std::string matName = LayerName + "_Material";
  Acts::Transform3 base{Acts::Transform3::Identity()};
  cont->addMaterial(matName.c_str(),
    [&](Acts::Experimental::MaterialDesignatorBlueprintNode& mat) {
      mat.configureFace(Acts::CylinderVolumeBounds::Face::NegativeDisc,
                        {Acts::AxisDirection::AxisR,   Acts::AxisBoundaryType::Bound, 100},
                        {Acts::AxisDirection::AxisPhi, Acts::AxisBoundaryType::Bound, 100});
      mat.configureFace(Acts::CylinderVolumeBounds::Face::PositiveDisc,
                        {Acts::AxisDirection::AxisR,   Acts::AxisBoundaryType::Bound, 100},
                        {Acts::AxisDirection::AxisPhi, Acts::AxisBoundaryType::Bound, 100});
      mat.addCylinderContainer(LayerName, Acts::AxisDirection::AxisZ,
        [&](auto& L) {
          L.addLayer(LayerName, [&](auto& layer) {
            makeLayerFunc(
              base * Acts::Translation3{Acts::Vector3{0, 0, z_pos}},
              layer, LayerName, bin0, bin1);
          });
        });
    });
}

// ==================================================================================
// AddCylinderLayer_and_Material
// Wraps a barrel cylinder layer (TBPX or T2B) in a material designator.
// ==================================================================================
void AddCylinderLayer_and_Material(Acts::Experimental::CylinderContainerBlueprintNode* cont,
                                    const std::string& LayerName,
                                    KdtSurfacesDim2Bin100& KdtSurfaces,
                                    double bin0, double bin1,
                                    bool isOT = false) {
  std::string matName = LayerName + "_Material";
  Acts::Transform3 base{Acts::Transform3::Identity()};
  cont->addMaterial(matName.c_str(),
    [&](Acts::Experimental::MaterialDesignatorBlueprintNode& mat) {
      mat.configureFace(Acts::CylinderVolumeBounds::Face::OuterCylinder,
                        {Acts::AxisDirection::AxisRPhi, Acts::AxisBoundaryType::Bound, 100},
                        {Acts::AxisDirection::AxisZ,    Acts::AxisBoundaryType::Bound, 100});
      mat.addCylinderContainer(LayerName, Acts::AxisDirection::AxisR,
        [&](auto& L) {
          L.addLayer(LayerName, [&](auto& layer) {
            auto surfaces = SelectActiveSurfaces_Phase2(KdtSurfaces, LayerName);
            makeBinning(layer, Acts::SurfaceArrayNavigationPolicy::LayerType::Cylinder,
                        bin0, bin1);
            layer.setSurfaces(surfaces)
                 .setLayerType(Acts::Experimental::LayerBlueprintNode::LayerType::Cylinder)
                 .setEnvelope(Acts::ExtentEnvelope{{
                     .z = {5 * Acts::UnitConstants::mm, 5 * Acts::UnitConstants::mm},
                     .r = {1 * Acts::UnitConstants::mm, 1 * Acts::UnitConstants::mm},
                 }})
                 .setTransform(base);
            // OT barrel layers have no well-defined centre of gravity due to
            // stacked-module geometry — disable CoG fitting on all axes.
            if (isOT) layer.setUseCenterOfGravity(false, false, false);
          }).setUseCenterOfGravity(false, false, false);
        });
    });
}

// ==================================================================================
// Material surface visitor (identical to Phase-1)
// ==================================================================================
struct MatSurfaceSelector : public Acts::TrackingGeometryMutableVisitor {
  std::vector<Acts::Surface*> surfaces;
  void visitSurface(Acts::Surface& surface) override {
    if (surface.surfaceMaterial() != nullptr &&
        !rangeContainsValue(surfaces, &surface))
      surfaces.push_back(&surface);
  }
};

// ==================================================================================
// Material validation helpers (identical to Phase-1)
// ==================================================================================
struct MaterialValidation_cfg {
  std::size_t ntracks                               = 1000;
  Acts::Vector3 startPosition                       = Acts::Vector3(0., 0., 0.);
  std::pair<double, double> phiRange                = {-std::numbers::pi, std::numbers::pi};
  std::pair<double, double> etaRange                = {-4., 4.};
  std::shared_ptr<Acts::MaterialValidater> materialValidater = nullptr;
  std::string outputMaterialTracks                  = "material_tracks";
};

using RandomEngine = std::mt19937;
using RandomSeed   = uint32_t;

class MaterialValidator {
public:
  explicit MaterialValidator(const MaterialValidation_cfg& cfg) : m_cfg(cfg) {
    if (!m_cfg.materialValidater)
      throw std::invalid_argument("Missing material validater.");
  }
  std::unordered_map<std::size_t, Acts::RecordedMaterialTrack>
  execute(const myContext& context, bool debug = false) {
    RandomEngine rng(1234567890u + context.EvNumber);
    std::uniform_real_distribution<double> phiDist(m_cfg.phiRange.first, m_cfg.phiRange.second);
    std::uniform_real_distribution<double> etaDist(m_cfg.etaRange.first, m_cfg.etaRange.second);
    for (std::size_t i = 0; i < m_cfg.ntracks; ++i) {
      if (debug) std::cout << "\rTrack " << i + 1 << "/" << m_cfg.ntracks << std::flush;
      double phi   = phiDist(rng);
      double eta   = etaDist(rng);
      double theta = 2 * std::atan(std::exp(-eta));
      Acts::Vector3 dir(std::cos(phi) * std::sin(theta),
                        std::sin(phi) * std::sin(theta),
                        std::cos(theta));
      recordedMaterialTracks[i] = m_cfg.materialValidater->recordMaterial(
          context.geoContext, context.magFieldContext, m_cfg.startPosition, dir);
    }
    return recordedMaterialTracks;
  }
private:
  MaterialValidation_cfg m_cfg;
  std::unordered_map<std::size_t, Acts::RecordedMaterialTrack> recordedMaterialTracks;
};

struct MyMaterialTrackWriter_cfg {
  std::string inputMaterialTracks = "material-tracks";
  std::string filePath            = "";
  std::string fileMode            = "RECREATE";
  std::string treeName            = "material-tracks";
  bool recalculateTotals          = false;
  bool prePostStep                = false;
  bool storeSurface               = false;
  bool storeVolume                = false;
  bool collapseInteractions       = false;
};

class MyMaterialTrackWriter {
public:
  explicit MyMaterialTrackWriter(const MyMaterialTrackWriter_cfg& config)
      : m_cfg(config) {
    ActsPlugins::RootMaterialTrackIo::Config ioCfg;
    m_payload = std::make_unique<ActsPlugins::RootMaterialTrackIo>(ioCfg);
    if (m_cfg.inputMaterialTracks.empty())
      throw std::invalid_argument("Missing input collection");
    if (m_cfg.treeName.empty())
      throw std::invalid_argument("Missing tree name");
    m_outputFile = TFile::Open(m_cfg.filePath.c_str(), m_cfg.fileMode.c_str());
    if (!m_outputFile)
      throw std::ios_base::failure("Could not open '" + m_cfg.filePath + "'");
    m_outputFile->cd();
    m_outputTree = new TTree(m_cfg.treeName.c_str(), "TTree from RootMaterialTrackWriter");
    if (!m_outputTree) throw std::bad_alloc();
    m_payload->connectForWrite(*m_outputTree);
  }
  ~MyMaterialTrackWriter() { m_outputTree = nullptr; m_outputFile = nullptr; }
  void finalize() {
    if (m_outputFile && m_outputTree) {
      m_outputFile->cd(); m_outputTree->Write(); m_outputFile->Close();
    }
    m_outputTree = nullptr; m_outputFile = nullptr;
  }
  void writeT(const myContext& ctx,
              const std::unordered_map<std::size_t, Acts::RecordedMaterialTrack>& tracks) {
    for (auto& [id, t] : tracks) {
      m_payload->write(ctx.geoContext, ctx.EvNumber, t);
      m_outputTree->Fill();
    }
  }
private:
  MyMaterialTrackWriter_cfg m_cfg;
  TFile*  m_outputFile = nullptr;
  TTree*  m_outputTree = nullptr;
  std::unique_ptr<ActsPlugins::RootMaterialTrackIo> m_payload;
};

// ==================================================================================
// Matrix comparison utilities (kept for debugging stacked-module rotations)
// ==================================================================================
bool isTranspose(const Eigen::Matrix3d& A, const Eigen::Matrix3d& B, double eps = 1e-3) {
  return A.isApprox(B.transpose(), eps);
}
bool isEqual(const Eigen::MatrixXd& A, const Eigen::MatrixXd& B, double eps = 1e-3) {
  return A.isApprox(B, eps);
}
bool isInverse(const Eigen::MatrixXd& A, const Eigen::MatrixXd& B, double eps = 1e-3) {
  if (A.rows() != A.cols() || B.rows() != B.cols() || A.rows() != B.rows()) return false;
  Eigen::MatrixXd I = Eigen::MatrixXd::Identity(A.rows(), A.cols());
  return (A * B).isApprox(I, eps) && (B * A).isApprox(I, eps);
}

// ==================================================================================
//  ACTSTrackingGeometryProducer_Phase2
// ==================================================================================
class ACTSTrackingGeometryProducer_Phase2 : public edm::ESProducer {
public:
  explicit ACTSTrackingGeometryProducer_Phase2(const edm::ParameterSet& ps);
  ~ACTSTrackingGeometryProducer_Phase2() override = default;

  std::shared_ptr<TrackingGeometryWithDetEls>produce(const ACTSTrackerGeometryRecord& iRecord);

private:
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> trackerGeomToken_;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd>        trackerTopoToken_;
  edm::ESGetToken<Alignments,      TrackerAlignmentRcd>       trackerAlignToken_;

  bool        saveObjfile_, saveSvgfile_, mapMaterial_;
  std::string outputObjFile_, outputSvgFile_, materialFile_;
  std::vector<double> rangeZ_, rangeR_;
};

// ==================================================================================
// Constructor
// ==================================================================================
ACTSTrackingGeometryProducer_Phase2::ACTSTrackingGeometryProducer_Phase2(
    const edm::ParameterSet& ps)
    : saveObjfile_(ps.getUntrackedParameter<bool>("saveObjfile")),
      saveSvgfile_(ps.getUntrackedParameter<bool>("saveSvgfile")),
      mapMaterial_(ps.getUntrackedParameter<bool>("mapMaterial")),
      outputObjFile_(ps.getUntrackedParameter<std::string>("outputObjFile")),
      outputSvgFile_(ps.getUntrackedParameter<std::string>("outputSvgFile")),
      materialFile_(ps.getUntrackedParameter<std::string>("MaterialMaps")),
      rangeZ_(ps.getUntrackedParameter<std::vector<double>>("rangeZ")),
      rangeR_(ps.getUntrackedParameter<std::vector<double>>("rangeR")) {
  auto cc         = setWhatProduced(this);
  trackerGeomToken_  = cc.consumes();
  trackerTopoToken_  = cc.consumes();
  trackerAlignToken_ = cc.consumes();
}

// ==================================================================================
// produce()
// ==================================================================================
std::shared_ptr<TrackingGeometryWithDetEls>
ACTSTrackingGeometryProducer_Phase2::produce(const ACTSTrackerGeometryRecord& iRecord) {

  const TrackerGeometry& trackerGeom  = iRecord.get(trackerGeomToken_);
  const TrackerTopology& trackerTopo  = iRecord.get(trackerTopoToken_);
  const Alignments&      trackerAlign = iRecord.get(trackerAlignToken_);
  (void)trackerAlign; // alignment baked into surface transforms

  DetElVect DetEl_vector;

  // Local frame reference points for axis extraction
  const Local2DPoint center(0., 0.);
  const Local3DPoint locz(0., 0., 1.);
  const Local3DPoint locx(1., 0., 0.);

  // ============================================================
  // Loop over all sensitive detector units
  // ============================================================
  for (const auto& det : trackerGeom.detUnits()) {

    auto&       cmssw_surf = det->surface();
    const DetId ID         = det->geographicalId();

    // ----- Compute orthonormal rotation via local-frame vectors -----
    GlobalPoint position = trackerGeom.idToDet(ID)->toGlobal(center);
    GlobalPoint zpos     = trackerGeom.idToDet(ID)->toGlobal(locz);
    GlobalPoint xpos     = trackerGeom.idToDet(ID)->toGlobal(locx);

    GlobalVector dz = zpos - position;
    GlobalVector dx = xpos - position;

    Eigen::Vector3d dxV(dx.x(), dx.y(), dx.z());
    Eigen::Vector3d dVz(dz.x(), dz.y(), dz.z());
    dxV.normalize();
    dVz.normalize();
    dxV = dxV - dxV.dot(dVz) * dVz; // Gram–Schmidt: make dx ⊥ dz
    dxV.normalize();
    Eigen::Vector3d dyV = dVz.cross(dxV);

    Eigen::Matrix3d Rot;
    Rot.col(0) = dxV;
    Rot.col(1) = dyV;
    Rot.col(2) = dVz;

    Acts::Transform3 t = Acts::Transform3::Identity();
    t.prerotate(Rot);
    // CMSSW positions are in cm; ACTS expects mm → multiply by 10
    t.pretranslate(Acts::Vector3(position.x() * 10.,
                                 position.y() * 10.,
                                 position.z() * 10.));

    // ----- Identify subdetector -----
    std::string subDet;
    const int   sid = ID.subdetId();
    if      (sid == PixelSubdetector::PixelBarrel) subDet = "TBPX";
    else if (sid == PixelSubdetector::PixelEndcap) {
      // Distinguish TFPX (|disk| <= 12) from TEPX (|disk| > 12)
      // NOTE: the disk count boundary depends on the geometry tag.
      int disk = trackerTopo.pxfDisk(ID);
      subDet = (disk <= 12) ? "TFPX" : "TEPX";
    }
    else if (sid == Phase2OT::Barrel) subDet = "T2B";
    else if (sid == Phase2OT::Endcap) subDet = "T2E";
    else continue; // skip any service/support structures that sneak in

    // ----- Retrieve surface bounds -----
    std::shared_ptr<const Acts::PlanarBounds> bounds;
    auto& surfBounds = cmssw_surf.bounds();

    if (const auto* rect = dynamic_cast<const RectangularPlaneBounds*>(&surfBounds)) {
      double hx = rect->width()  * 10. / 2.; // cm → mm, half-width
      double hy = rect->length() * 10. / 2.; // cm → mm, half-length
      bounds = std::make_shared<Acts::RectangleBounds>(hx, hy);
    } else if (const auto* trap = dynamic_cast<const TrapezoidalPlaneBounds*>(&surfBounds)) {
      double hxMin = trap->widthAtHalfLength()   * 10.;  // bottom half-width
      double hxMax = trap->width()               * 10. / 2.; // top half-width (approx)
      double hy    = trap->length()              * 10. / 2.;
      // NOTE: for PS endcap modules the exact trapezoidal parameters should be
      // read from trackerTopo. This is a conservative approximation.
      bounds = std::make_shared<Acts::TrapezoidBounds>(hxMin, hxMax, hy);
    } else {
      std::cerr << "[Phase2] Unknown bounds type for DetId " << ID.rawId()
                << " (" << subDet << ") — skipping module.\n";
      continue;
    }

    // ----- Build the Acts::CMSDetectorElement -----

    // 1. Build the surface first (combines your transform + bounds)
/* auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
    std::make_shared<Acts::Transform3>(t),
    bounds 
); */

auto surface = Acts::Surface::makeShared<Acts::PlaneSurface>(Acts:: Transform3 transform1, bounds);



// 2. Fill the struct
Acts::CMSDetectorElementData detData;
detData.surf_        = surface;
detData.thickness_   = 0.03 * Acts::UnitConstants::mm;
detData.trans_       = t;              // Transform3 by value, not shared_ptr
detData.detID_       = ID.rawId();     // ← this is where your commented-out ID goes!
detData.subDetector_ = "Phase2Tracker"; // or "Pixel", "OT", etc. — whatever fits your geometry

DetEl_vector.push_back(std::make_shared<Acts::CMSDetectorElement>(detData)); 

//auto detEl = std::make_shared<Acts::CMSDetectorElement>(detData);
//DetEl_vector.push_back(detEl);

/*     auto detEl = std::make_shared<Acts::CMSDetectorElement>(
        ActsPlugins::TGeoDetectorElement::identifier(ID.rawId()),
        std::make_shared<Acts::Transform3>(t),
        bounds,
        0.03 * Acts::UnitConstants::mm  // nominal thickness (300 µm pixel / 300 µm strip)
    );
    DetEl_vector.push_back(detEl); */
  } // end loop over detUnits
 
  std::cout << "[Phase2] Created " << DetEl_vector.size() << " ACTS detector elements.\n";

  // ============================================================
  // Build KDT index over all active surfaces
  // ============================================================
  std::vector<std::shared_ptr<Acts::Surface>> allSurfaces;
  allSurfaces.reserve(DetEl_vector.size());
  for (const auto& de : DetEl_vector){
    allSurfaces.push_back(de->surface().getSharedPtr());

  }

  const Acts::GeometryContext viewcontext; 
  KdtSurfacesDim2Bin100 kdtSurfaces(viewcontext, allSurfaces, casts); //This function takes 4 arguments

  Acts::GeometryContext geoCtx;

  // ============================================================
  // Optional: decorate with pre-mapped material
  // ============================================================
/*   std::shared_ptr<const Acts::IMaterialDecorator> matDecorator = nullptr;
  if (!materialFile_.empty() && !mapMaterial_) {
    Acts::MaterialMapJsonConverter::Config matCfg; //Changed form Acts::JsonMaterialDecorator::Config since Config is not a member of this class 
    matCfg.inputMaterialTracks = materialFile_; //Changed from input 
    matDecorator = std::make_shared<Acts::JsonMaterialDecorator>(matCfg, Acts::Logging::INFO);
    std::cout << "[Phase2] Loaded material map from " << materialFile_ << "\n";
  } */

  // ============================================================
  // Disc layer builder lambda (shared by IT forward and OT endcap)
  // ============================================================
  auto makeDiscLayer = [&](Acts::Transform3 layerTransform,
                            auto& layer,
                            const std::string& name,
                            double bin0, double bin1) {
    auto surfaces = SelectActiveSurfaces_Phase2(kdtSurfaces, name);
    makeBinning(layer, Acts::SurfaceArrayNavigationPolicy::LayerType::Disc, bin0, bin1);
    layer.setSurfaces(surfaces)
         .setLayerType(Acts::Experimental::LayerBlueprintNode::LayerType::Disc)
         .setEnvelope(Acts::ExtentEnvelope{{
             .z = {5 * Acts::UnitConstants::mm, 5 * Acts::UnitConstants::mm},
             .r = {5 * Acts::UnitConstants::mm, 5 * Acts::UnitConstants::mm},
         }})
         .setTransform(layerTransform);
  };

  // ============================================================
  // Approximate disc Z positions (mm) for Blueprint translation.
  // Keep in sync with the KDT ranges above.
  // ============================================================
  // TFPX disc Z centres (positive side)
  const std::array<double,12> tfpxZ = {293,400,505,620,735,857,981,1100,1210,1290,1372,1452};
  // TEPX disc Z centres (positive side)
  const std::array<double, 4> tepxZ = {1670, 1785, 1900, 1965};
  // T2E disc Z centres (positive side)
  const std::array<double, 5> t2eZ  = {1322, 1572, 1842, 2112, 2380};

  // ============================================================
  // ACTS Blueprint construction
  //
  // Structure:
  //   World
  //    └─ Tracker (CylinderContainer, AxisZ)
  //        ├─ InnerTracker (CylinderContainer, AxisR)
  //        │   ├─ TEPX_Neg  (CylinderContainer, AxisZ) — 4 discs
  //        │   ├─ TFPX_Neg  (CylinderContainer, AxisZ) — 12 discs
  //        │   ├─ TBPX      (CylinderContainer, AxisR) — 4 cylinders
  //        │   ├─ TFPX_Pos  (CylinderContainer, AxisZ) — 12 discs
  //        │   └─ TEPX_Pos  (CylinderContainer, AxisZ) — 4 discs
  //        └─ OuterTracker (CylinderContainer, AxisR)
  //            ├─ T2E_Neg   (CylinderContainer, AxisZ) — 5 discs
  //            ├─ T2B       (CylinderContainer, AxisR) — 6 cylinders
  //            └─ T2E_Pos   (CylinderContainer, AxisZ) — 5 discs
  // ============================================================
  Acts::Experimental::Blueprint::Config blueprintCfg;
  auto blueprint = std::make_unique<Acts::Experimental::Blueprint>(blueprintCfg); 
  Acts::Transform3 base{Acts::Transform3::Identity()};

  blueprint->addCylinderContainer("Tracker", Acts::AxisDirection::AxisZ,
    [&](auto& tracker) {
      tracker.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
             .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);

      // ======== INNER TRACKER ========
      tracker.addCylinderContainer("InnerTracker", Acts::AxisDirection::AxisR,
        [&](auto& IT) {
          IT.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
            .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);

          // ---- TEPX Negative ----
          IT.addCylinderContainer("TEPX_Neg", Acts::AxisDirection::AxisZ,
            [&](auto& tepxNeg) {
              tepxNeg.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
                     .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
              for (int d = 3; d >= 0; --d) {
                std::string name = "TEPX_Neg" + std::to_string(d);
                AddDiskLayer_and_Material(&tepxNeg, name, makeDiscLayer,
                                         -tepxZ[d], 40, 50);
              }
            });

          // ---- TFPX Negative ----
          IT.addCylinderContainer("TFPX_Neg", Acts::AxisDirection::AxisZ,
            [&](auto& tfpxNeg) {
              tfpxNeg.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
                     .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
              for (int d = 11; d >= 0; --d) {
                std::string name = "TFPX_Neg" + std::to_string(d);
                AddDiskLayer_and_Material(&tfpxNeg, name, makeDiscLayer,
                                         -tfpxZ[d], 40, 60);
              }
            });

          // ---- TBPX (4 barrel layers) ----
          IT.addCylinderContainer("TBPX", Acts::AxisDirection::AxisR,
            [&](auto& tbpx) {
              tbpx.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
                  .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
              AddCylinderLayer_and_Material(&tbpx, "TBPX_L0", kdtSurfaces, 50, 100, false);
              AddCylinderLayer_and_Material(&tbpx, "TBPX_L1", kdtSurfaces, 50, 100, false);
              AddCylinderLayer_and_Material(&tbpx, "TBPX_L2", kdtSurfaces, 50, 100, false);
              AddCylinderLayer_and_Material(&tbpx, "TBPX_L3", kdtSurfaces, 50, 100, false);
            });

          // ---- TFPX Positive ----
          IT.addCylinderContainer("TFPX_Pos", Acts::AxisDirection::AxisZ,
            [&](auto& tfpxPos) {
              tfpxPos.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
                     .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
              for (int d = 0; d < 12; ++d) {
                std::string name = "TFPX_Pos" + std::to_string(d);
                AddDiskLayer_and_Material(&tfpxPos, name, makeDiscLayer,
                                         tfpxZ[d], 40, 60);
              }
            });

          // ---- TEPX Positive ----
          IT.addCylinderContainer("TEPX_Pos", Acts::AxisDirection::AxisZ,
            [&](auto& tepxPos) {
              tepxPos.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
                     .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
              for (int d = 0; d < 4; ++d) {
                std::string name = "TEPX_Pos" + std::to_string(d);
                AddDiskLayer_and_Material(&tepxPos, name, makeDiscLayer,
                                         tepxZ[d], 40, 50);
              }
            });
        }); // end InnerTracker

      // ======== OUTER TRACKER ========
      tracker.addCylinderContainer("OuterTracker", Acts::AxisDirection::AxisR,
        [&](auto& OT) {
          OT.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
            .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);

          // ---- T2E Negative (5 discs) ----
          OT.addCylinderContainer("T2E_Neg", Acts::AxisDirection::AxisZ,
            [&](auto& t2eNeg) {
              t2eNeg.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
                    .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
              for (int d = 4; d >= 0; --d) {
                std::string name = "T2E_Neg" + std::to_string(d);
                AddDiskLayer_and_Material(&t2eNeg, name, makeDiscLayer,
                                         -t2eZ[d], 40, 100);
              }
            });

          // ---- T2B barrel (6 layers of PS/2S stacked modules) ----
          OT.addCylinderContainer("T2B", Acts::AxisDirection::AxisR,
            [&](auto& t2b) {
              t2b.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
                 .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
              for (int l = 0; l < 6; ++l) {
                std::string name = "T2B_L" + std::to_string(l);
                // T2B stacked modules: disable CoG fitting (isOT=true)
                AddCylinderLayer_and_Material(&t2b, name, kdtSurfaces, 60, 120, true);
              }
            });

          // ---- T2E Positive (5 discs) ----
          OT.addCylinderContainer("T2E_Pos", Acts::AxisDirection::AxisZ,
            [&](auto& t2ePos) {
              t2ePos.setAttachmentStrategy(Acts::VolumeAttachmentStrategy::Gap)
                    .setResizeStrategy(Acts::VolumeResizeStrategy::Gap);
              for (int d = 0; d < 5; ++d) {
                std::string name = "T2E_Pos" + std::to_string(d);
                AddDiskLayer_and_Material(&t2ePos, name, makeDiscLayer,
                                         t2eZ[d], 40, 100);
              }
            });
        }); // end OuterTracker

    }); // end Tracker

  // ============================================================
  // Construct the Acts::TrackingGeometry from the Blueprint
  // ============================================================
  Acts::Experimental::BlueprintOptions BluePrint_otp; 
  auto logger = Acts::getDefaultLogger("UnitTests", Acts::Logging::INFO);
  auto trackingGeometry = (*blueprint).construct(BluePrint_otp, geoCtx, *logger);
  //auto trackingGeometry = (*blueprint).construct(BluePrint_otp, geoCtx, Acts::Logging::INFO, matDecorator.get());
  //std::shared_ptr<Acts::TrackingGeometry> trackingGeometry = std::move(root->construct(blueprint, geoCtx, *logger)); 
  std::cout << "[Phase2] TrackingGeometry constructed successfully.\n";

  //From Lorenzo's Code                                                
  /* Acts::GeometryContext gctx;
  auto logger = Acts::getDefaultLogger("UnitTests", Acts::Logging::INFO);
  Acts::Experimental::BlueprintOptions BluePrint_otp;
  std::shared_ptr<Acts::TrackingGeometry> trackingGeometry = std::move(root->construct(BluePrint_otp, gctx, *logger));
  std::cout << "[Phase2] TrackingGeometry constructed successfully.\n"; */

  // ============================================================
  // Optional: material mapping workflow
  // ============================================================
 /*  if (mapMaterial_) {
    std::cout << "[Phase2] Starting material mapping...\n";
    // 1. Collect all surfaces that carry a material proxy
    MatSurfaceSelector sel;
    trackingGeometry->visitMutableSurfaces(sel);
    std::cout << "[Phase2] Found " << sel.surfaces.size()
              << " surfaces for material mapping.\n";

    // 2. Set up BinnedSurfaceMaterialAccumulater
    Acts::BinnedSurfaceMaterialAccumulater::Config accCfg;
    accCfg.materialSurfaces = {sel.surfaces.begin(), sel.surfaces.end()};
    auto accumulater = std::make_shared<Acts::BinnedSurfaceMaterialAccumulater>(
        accCfg, Acts::getDefaultLogger("BinnedMatAcc", Acts::Logging::INFO));

    // 3. Set up MaterialMapper (PropagatorMaterialAssigner)
    // NOTE: wire your propagator here — a StraightLineStepper + Navigator is
    // sufficient for geometry-only material mapping.
    // Acts::MaterialMapper::Config mmCfg;
    // mmCfg.materialAssigner  = std::make_shared<Acts::PropagatorMaterialAssigner>(...);
    // mmCfg.materialAccumulater = accumulater;
    // auto mapper = std::make_shared<Acts::MaterialMapper>(mmCfg, ...);

    // 4. Read material tracks and run mapping
    // MaterialConfig mCfg;
    // mCfg.fileList = { "material_tracks.root" };
    // MyMaterialEvReader reader(mCfg);
    // for (std::size_t ev = 0; ev < nEvents; ++ev) {
    //   auto tracks = reader.read(ev);
    //   myContext ctx; ctx.EvNumber = ev;
    //   mapper->finalizeMaps(ctx.geoContext, tracks);
    // }

    // 5. Write JSON material maps
    // ACTSinCMSSW::JsonMaterialWriter writer;
    // writer.write(*trackingGeometry);
    std::cout << "[Phase2] Material mapping placeholder — wire your propagator above.\n";
  }

  // ============================================================
  // Optional: write OBJ visualisation
  // ============================================================
  if (saveObjfile_) {
    writeSurfacesObj(allSurfaces, outputObjFile_);
    std::cout << "[Phase2] OBJ written to " << outputObjFile_ << "\n";
  }

  // ============================================================
  // Optional: write SVG visualisation
  // ============================================================
  if (saveSvgfile_) {
    // ActsPlugins::TrackingGeometrySvgConverter::Config svgCfg;
    // ... configure and write to outputSvgFile_
    std::cout << "[Phase2] SVG output not yet wired — configure svgCfg above.\n";
  } */

  // ============================================================
  // Pack and return
  // ============================================================
  auto result = std::make_shared<TrackingGeometryWithDetEls>();
  result->detElements      = std::move(DetEl_vector);
  result->trackingGeometry = std::move(trackingGeometry);
  return result;
}

// ==================================================================================
// Registration
// ==================================================================================
DEFINE_FWK_EVENTSETUP_MODULE(ACTSTrackingGeometryProducer_Phase2);