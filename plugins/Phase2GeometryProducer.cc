/* 
//Steps of implementation:- 
// + Material methods: Material Config, Material reader.
// + SelectActiveSurfaces_PhaseII()
// + AddExtraLayer()
// + 
// + Blueprint tree: Construct my layers and my volumes
// + Construct the TrackingGeometry from the Blueprint 
// + Material Decorator

//FrameWork Core header files 
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Utilities/interface/typelookup.h"
#include "FWCore/Framework/interface/MakerMacros.h" 

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h" //The file is not found somehow 
#include "Geometry/Records/interface/ACTSTrackerGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "CondFormats/AlignmentRecord/interface/TrackerAlignmentRcd.h"
#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/Alignment/interface/AlignTransform.h"
#include "Alignment/CommonAlignment/interface/Alignable.h"
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include "DataFormats/TrackerCommon/interface/PixelBarrelName.h"
#include "DataFormats/TrackerCommon/interface/PixelEndcapName.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DetectorDescription/Core/interface/DDRotationMatrix.h"
#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "Math/RotationZ.h"
#include "FWCore/Framework/interface/ESConsumesCollector.h"

#include "Math/Rotation3D.h"
#include "Math/AxisAngle.h"

//ACTS and ACTS plugins header files 
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Detector/KdtSurfacesProvider.hpp"
#include "Acts/Geometry/Polyhedron.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/TrackingGeometryVisitor.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/TrapezoidBounds.hpp"
#include "Acts/Visualization/GeometryView3D.hpp"
#include "Acts/Visualization/ObjVisualization3D.hpp"
#include "ActsPlugins/Json/JsonDetectorElement.hpp"
#include "Acts/Visualization/ViewConfig.hpp"

#include "Acts/Geometry/Blueprint.hpp"
#include "Acts/Geometry/ContainerBlueprintNode.hpp" 
#include "Acts/Geometry/LayerBlueprintNode.hpp" 
#include "Acts/Geometry/MaterialDesignatorBlueprintNode.hpp" 
#include "Acts/Geometry/SurfaceArrayCreator.hpp"
#include "Acts/Detector/KdtSurfacesProvider.hpp"
#include "Acts/Detector/ProtoDetector.hpp"
#include "Acts/Detector/detail/ReferenceGenerators.hpp"
#include "Acts/Detector/interface/ISurfacesProvider.hpp"
#include "Acts/Detector/LayerStructureBuilder.hpp"
#include "Acts/Detector/detail/BlueprintHelper.hpp"
#include "Acts/Detector/detail/BlueprintDrawer.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Navigation/SurfaceArrayNavigationPolicy.hpp"
#include "Acts/Navigation/TryAllNavigationPolicy.hpp"
#include "ActsPlugins/ActSVG/DetectorSvgConverter.hpp"
#include "ActsPlugins/Json/JsonSurfacesReader.hpp"
#include "ActsPlugins/Json/JsonMaterialDecorator.hpp"
#include "ActsPlugins/Json/SurfaceJsonConverter.hpp"
#include "ActsPlugins/ActSVG/TrackingGeometrySvgConverter.hpp"
#include "ActsPlugins/ActSVG/SurfaceArraySvgConverter.hpp"
#include "ActsPlugins/Root/RootMaterialTrackIo.hpp"

#include "Acts/Material/PropagatorMaterialAssigner.hpp"
#include "Acts/Material/BinnedSurfaceMaterialAccumulater.hpp"
#include "Acts/Material/BinnedSurfaceMaterial.hpp"
#include "Acts/Material/AccumulatedMaterialSlab.hpp"
#include "Acts/Material/HomogeneousSurfaceMaterial.hpp"
#include "Acts/Material/IntersectionMaterialAssigner.hpp"
#include "Acts/Material/MaterialValidater.hpp"
#include "Acts/Material/MaterialMapper.hpp"

//C++ header files 
#include <fstream>
#include <iomanip>
#include <random>
#include <nlohmann/json.hpp> 
#include "TChain.h"
#include "TFile.h"

#include <iostream>
#include <string>
#include <vector>

//Custom header files 
#include "ACTSPhase2InCMSSW/ACTSPhase2InCMSSW/interface/CMSDetectorElement.hpp" 
#include "ACTSPhase2InCMSSW/ACTSPhase2InCMSSW/interface/JsonMaterialWriter.hpp"

using json = nlohmann::json;
using KdtSurfacesDim2Bin100 = Acts::Experimental::KdtSurfaces<2u, 100u>; //Why bin = 100? 
const std::array<Acts::AxisDirection, 2UL> casts{Acts::AxisDirection::AxisZ, Acts::AxisDirection::AxisR}; //Set the cylindrical coordinates for this code 
using DetElVect = std::vector<std::shared_ptr<Acts::CMSDetectorElement>>; //A custom class for CMS detector elements in ACTS

struct TrackingGeometryWithDetEls {
    DetElVect detElements;
    std::shared_ptr<Acts::TrackingGeometry> trackingGeometry;
};

//A root pattern that allows us to dynamically create objects 
TYPELOOKUP_DATA_REG(TrackingGeometryWithDetEls);

//+++++ Material Methods +++++//

struct MaterialConfig {
  /// material collection to read
  std::string outputMaterialTracks = "material-tracks";
  /// name of the output tree
  std::string treeName = "material-tracks";
  /// List of input files
  std::vector<std::string> fileList;

  bool readCachedSurfaceInformation = false;
};

class Phase2GeometryProducer : public edm::ESProducer {
public:
  explicit Phase2GeometryProducer(const edm::ParameterSet& ps);
  ~Phase2GeometryProducer() override = default;
  std::shared_ptr<TrackingGeometryWithDetEls> produce(const ACTSTrackerGeometryRecord& iRecord);

private:
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> trackerGeomToken_;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> trackerTopoToken_;
  edm::ESGetToken<Alignments, TrackerAlignmentRcd> trackerAlignToken_;

  bool saveObjfile_, saveSvgfile_, mapMaterial_;
  std::string outputObjFile_, outputSvgFile_, materialFile_;
  std::vector<double> rangeZ_;
  std::vector<double> rangeR_;
};

 //What do we need this sorting for? 
template <typename element_t, typename index_t>
void stableSort(index_t numElements, const element_t* elements,
                index_t* sortedIndices, Bool_t sortDescending) {
  for (index_t i = 0; i < numElements; i++) {
    sortedIndices[i] = i;
  }

  if (sortDescending) {
    std::stable_sort(sortedIndices, sortedIndices + numElements,
                     CompareDesc<const element_t*>(elements));
  } else {
    std::stable_sort(sortedIndices, sortedIndices + numElements,
                     CompareAsc<const element_t*>(elements));
  }
}

class MyMaterialEvReader {
public:
    MyMaterialEvReader(const MaterialConfig& config) : m_cfg(config){

      ActsPlugins::RootMaterialTrackIo::Config ioCfg;
      m_payload = std::make_unique<ActsPlugins::RootMaterialTrackIo>(ioCfg);
      
      m_inputChain = std::make_unique<TChain>(m_cfg.treeName.c_str());

      // loop over the input files
      for (const auto& inputFile : m_cfg.fileList) {
        // add file to the input chain
        m_inputChain->Add(inputFile.c_str());
        //std::cout << "Adding File " << inputFile << " to tree '" << m_cfg.treeName << "'." << std::endl;
      }

      // Connect the branches
      m_payload->connectForRead(*m_inputChain);

      // get the number of entries, which also loads the tree
      std::size_t nentries = m_inputChain->GetEntries();
      m_events = static_cast<std::size_t>(m_inputChain->GetMaximum("event_id") + 1);
      m_batchSize = nentries / m_events;
      //std::cout << "The full chain has "
      //           << nentries << " entries for " << m_events
      //           << " events this corresponds to a batch size of: " << m_batchSize << std::endl;

      // Sort the entry numbers of the events
      {
        // necessary to guarantee that m_inputChain->GetV1() is valid for the
        // entire range
        m_inputChain->SetEstimate(nentries + 1);

        m_entryNumbers.resize(nentries);
        m_inputChain->Draw("event_id", "", "goff");
        stableSort(m_inputChain->GetEntries(), m_inputChain->GetV1(),
                                m_entryNumbers.data(), false);
      }
    }

    std::unordered_map<std::size_t, Acts::RecordedMaterialTrack> read(std::size_t eventNumber) {

      mtrackCollection.clear();
      
      // Loop over the entries for this event
      for (std::size_t ib = 0; ib < m_batchSize; ++ib) {
        // Read the correct entry: startEntry + ib
        auto entry = m_batchSize * eventNumber + ib;
        entry = m_entryNumbers.at(entry);
        //std::cout << "Reading event: " << eventNumber
        //                               << " with stored entry: " << entry << std::endl; 
        m_inputChain->GetEntry(entry);

        Acts::RecordedMaterialTrack rmTrack = m_payload->read();

        //std::cout << "Track vertex:  " << rmTrack.first.first << std::endl;
        //std::cout << "Track momentum:" << rmTrack.first.second << std::endl;

        mtrackCollection[ib] = (std::move(rmTrack));
      }

      return mtrackCollection;
    }

private:
    MaterialConfig m_cfg;
    std::unique_ptr<TChain> m_inputChain;
    std::unique_ptr<ActsPlugins::RootMaterialTrackIo> m_payload;
    
    std::vector<long long> m_entryNumbers = {};

    std::size_t m_events = 0;
    std::size_t m_batchSize = 0;

    std::unordered_map<std::size_t, Acts::RecordedMaterialTrack> mtrackCollection;
};


struct MyContext{
  /// Magnetic and Geometry context
  Acts::GeometryContext geoContext;
  Acts::MagneticFieldContext magFieldContext;
  /// Number of event
  std::size_t EvNumber; 
};

//A function to perform translation transformation given 3D cartesian coordinates 
Acts::Transform3 GenerateTranslation(double dx, double dy, double dz) {
    return Acts::Transform3::Identity() * Acts::Translation3{Acts::Vector3{dx, dy, dz}};
};

// FIX IT: //
//+ Remove the old layers from phase 1//
//+ Add the new layers of phase 2 //
//+ Find an automatic way to add the layers' boundaries // 

//I can for now do it manually, by plotting the geometry maps in the rz plane in the CMSSW and take the boundries to write them here//

std::vector<std::shared_ptr<Acts::Surface>> SelectActiveSurfaces_PhaseII(const KdtSurfacesDim2Bin100& surfaces, std::string Layer_name){

  //Acts::RangeXD<Dim, datatype, Vector/array> Range({z0, r0}, {z1, r1});
  //Initialize the range for the tracker layers 
  Acts::RangeXD<2, double, std::array> Range({0, 0}, {0, 0});

  if(Layer_name=="PixL0"){
    Range = Acts::RangeXD<2, double, std::array>({-290, 15}, {280, 35});
  }
  else if(Layer_name=="PixL1"){
    Range = Acts::RangeXD<2, double, std::array>({-290,40}, {280,80});
  }
  else if(Layer_name=="PixL2"){
    Range = Acts::RangeXD<2, double, std::array>({-290,85}, {280,150});
  }
  else if(Layer_name=="PixL3"){
    Range = Acts::RangeXD<2, double, std::array>({-290,150}, {280,200});
  }
  else if(Layer_name=="PixelPos2"){
    Range = Acts::RangeXD<2, double, std::array>({450,25}, {1160,220}); //550
  }
  else if(Layer_name=="PixelPos1"){
    Range = Acts::RangeXD<2, double, std::array>({350,25}, {445,220});
  }
  else if(Layer_name=="PixelPos0"){
    Range = Acts::RangeXD<2, double, std::array>({285,25}, {345,220});
  }
  else if(Layer_name=="PixelNeg2"){
    Range = Acts::RangeXD<2, double, std::array>({-1160,25}, {-450,220});
  }
  else if(Layer_name=="PixelNeg1"){
    Range = Acts::RangeXD<2, double, std::array>({-445,25}, {-350,220});
  }
  else if(Layer_name=="PixelNeg0"){
    Range = Acts::RangeXD<2, double, std::array>({-345,25}, {-285,220});
  }
  else if(Layer_name=="TIDNeg2"){
    Range = Acts::RangeXD<2, double, std::array>({-1160,220}, {-1000,530});
  }
  else if(Layer_name=="TIDNeg1"){
    Range = Acts::RangeXD<2, double, std::array>({-1000,220}, {-850,530});
  }
  else if(Layer_name=="TIDNeg0"){
    Range = Acts::RangeXD<2, double, std::array>({-850,220}, {-700,530});
  }
  else if(Layer_name=="TIB3"){
    Range = Acts::RangeXD<2, double, std::array>({-700,480}, {700,560});
  }
  else if(Layer_name=="TIB2"){
    Range = Acts::RangeXD<2, double, std::array>({-700,400}, {700,480});
  }
  else if(Layer_name=="TIB1"){
    Range = Acts::RangeXD<2, double, std::array>({-700,300}, {700,400});
  }
  else if(Layer_name=="TIB0"){
    Range = Acts::RangeXD<2, double, std::array>({-700,200}, {700,300});
  }
  else if(Layer_name=="TIDPos2"){
    Range = Acts::RangeXD<2, double, std::array>({1000,220}, {1160,530});
  }
  else if(Layer_name=="TIDPos1"){
    Range = Acts::RangeXD<2, double, std::array>({850,220}, {1000,530});
  }
  else if(Layer_name=="TIDPos0"){
    Range = Acts::RangeXD<2, double, std::array>({700,220}, {850,530});
  }
  else if(Layer_name=="NegTEC8"){
    Range = Acts::RangeXD<2, double, std::array>({  -2800,220}, {   -2550,1200});
  }
  else if(Layer_name=="NegTEC7"){
    Range = Acts::RangeXD<2, double, std::array>({  -2550,220}, {   -2350,1200});
  }
  else if(Layer_name=="NegTEC6"){
    Range = Acts::RangeXD<2, double, std::array>({  -2350,220}, {   -2150,1200});
  }
  else if(Layer_name=="NegTEC5"){
    Range = Acts::RangeXD<2, double, std::array>({  -2150,220}, {   -1950,1200});
  }
  else if(Layer_name=="NegTEC4"){
    Range = Acts::RangeXD<2, double, std::array>({  -1950,220}, {   -1810,1200});
  }
  else if(Layer_name=="NegTEC3"){
    Range = Acts::RangeXD<2, double, std::array>({  -1810,220}, {   -1675,1200});
  }
  else if(Layer_name=="NegTEC2"){
    Range = Acts::RangeXD<2, double, std::array>({  -1675,220}, {   -1525,1200});
  }
  else if(Layer_name=="NegTEC1"){
    Range = Acts::RangeXD<2, double, std::array>({  -1525,220}, {   -1400,1200});
  }
  else if(Layer_name=="NegTEC0"){
    Range = Acts::RangeXD<2, double, std::array>({  -1400,220}, {   -1200,1200});
  }
  else if(Layer_name=="TOB0"){
    Range = Acts::RangeXD<2, double, std::array>({  -1160,560}, {    1160, 650});
  }
  else if(Layer_name=="TOB1"){
    Range = Acts::RangeXD<2, double, std::array>({-1160, 650}, {1160, 750});
  }
  else if(Layer_name=="TOB2"){
    Range = Acts::RangeXD<2, double, std::array>({-1160, 750}, {1160, 840});
  }
  else if(Layer_name=="TOB3"){
    Range = Acts::RangeXD<2, double, std::array>({-1160, 840}, {1160, 940});
  }
  else if(Layer_name=="TOB4"){
    Range = Acts::RangeXD<2, double, std::array>({-1160, 940}, {1160, 1040});
  }
  else if(Layer_name=="TOB5"){
    Range = Acts::RangeXD<2, double, std::array>({-1160, 1040}, {1160, 1200});
  }
  else if(Layer_name=="PosTEC8"){
    Range = Acts::RangeXD<2, double, std::array>({  2550, 220}, {   2800,1200});
  }
  else if(Layer_name=="PosTEC7"){
    Range = Acts::RangeXD<2, double, std::array>({  2350, 220}, {   2550,1200});
  }
  else if(Layer_name=="PosTEC6"){
    Range = Acts::RangeXD<2, double, std::array>({  2150, 220}, {   2350,1200});
  }
  else if(Layer_name=="PosTEC5"){
    Range = Acts::RangeXD<2, double, std::array>({  1950, 220}, {   2150,1200});
  }
  else if(Layer_name=="PosTEC4"){
    Range = Acts::RangeXD<2, double, std::array>({  1810, 220}, {  1950,1200});
  }
  else if(Layer_name=="PosTEC3"){
    Range = Acts::RangeXD<2, double, std::array>({  1675, 220}, {   1810,1200});
  }
  else if(Layer_name=="PosTEC2"){
    Range = Acts::RangeXD<2, double, std::array>({  1525, 220}, {   1675,1200});
  }
  else if(Layer_name=="PosTEC1"){
    Range = Acts::RangeXD<2, double, std::array>({  1400, 220}, {   1525,1200});
  }
  else if(Layer_name=="PosTEC0"){
    Range = Acts::RangeXD<2, double, std::array>({  1200, 220}, {   1400,1200});
  }

  std::vector<std::shared_ptr<Acts::Surface>> ActiveSurfaces = surfaces.surfaces(Range);

  return ActiveSurfaces;
}

// what this thing does, so far it
void makeBinning(auto& layer, Acts::SurfaceArrayNavigationPolicy::LayerType layer_type, double Bin0, double Bin1){
  layer.setNavigationPolicyFactory(Acts::NavigationPolicyFactory().add<Acts::SurfaceArrayNavigationPolicy>(
                                Acts::SurfaceArrayNavigationPolicy::Config{ //The in 
                                    .layerType = layer_type,
                                    .bins = {Bin0, Bin1}}) 
                            .add<Acts::TryAllNavigationPolicy>(
                                Acts::TryAllNavigationPolicy::Config{.sensitives = true})    //Not including passive surfaces and not including portals                                       
                            .asUniquePtr()); 
}

//A function to add layers to a material 
void AddExtraLayer(std::string gapName, 
                   bool isBarrel, 
                   Acts::Experimental::CylinderContainerBlueprintNode* container,
                   Acts::Transform3 transform,
                   std::shared_ptr<Acts::CylinderVolumeBounds> bounds){

Acts::Transform3 base{Acts::Transform3::Identity()};
Acts::AxisDirection BuildDirection;

if(isBarrel){
  BuildDirection = Acts::AxisDirection::AxisR;
} else {
  BuildDirection = Acts::AxisDirection::AxisZ; 
}

container->addCylindarContainer(gapName, BuildDirection, [&](auto& gap){ //In the original code it was gapName.c_str() but gapName is already a string so why? 
gap.setAttachmentStategy(Acts::VolumeAttachmentStrategy::Gap).setResizeStategy(Acts::VolumeResizeStrategy::Gap);

gap.addMaterial(gapName, [&](Acts::Experimental::MaterialDesignatorBlueprintNode& material){
  if(isBarrel){
            //configureFace(CylinderVolumeBounds::Face face, const DirectedProtoAxis& loc0, const DirectedProtoAxis& loc1 )
            //configureFace(CylindarVolumeBounds::Face, {AxisDirection axisDir, AxisBoundaryType abType, std::size_t nbins}, loc1) 
    material.configureFace(Acts::CylindarVolumeBounds::Face::OuterCylinder, {Acts::AxisDirection::AxisRPhi, Acts::AxisBoundaryType::Bound, 100}, {Acts::AxisDirection::AxisZ, Acts::AxisBoundaryType::Bound, 100}); 
  } else {
    material.configureFace(Acts::CylinderVolumeBounds::Face::NegativeDisc, {Acts::AxisDirection::AxisR, Acts::AxisBoundaryType::Bound, 100}, {Acts::AxisDirection::AxisPhi, Acts::AxisBoundaryType::Bound, 100});
    material.configureFace(Acts::CylinderVolumeBounds::Face::PositiveDisc, {Acts::AxisDirection::AxisR, Acts::AxisBoundaryType::Bound, 100}, {Acts::AxisDirection::AxisPhi, Acts::AxisBoundaryType::Bound, 100});
  }

  material.addCylindarContainer(gapNmae, Acts::AxisDirection::AxisZ, [&](auto& L){L.addStaticVolume(base * transform, bounds, gapName); }); 
});

});                                                      

};

struct MatSurfaceSelector : public Acts::TrackingGeometryMutableVisitor {
  std::vector<Acts::Surface*> surfaces;
  void visitSurface(Acts::Surface& surface) override {
    if(surface.surfaceMaterial() != nullptr && !rangeContainsValue(surfaces, &surface)) {
        surfaces.push_back(&surface);
    }
  }
};

template <typename MakeLayerFn>
void AddDiskLayerAndMaterial(Acts::CylinderContainerBlueprintNode* container,
                             std:: string LayerName,
                             MakeLayerFn&& makeLayerFunc  //What does it mean to add two && to the function, a reference function?
                             double z_pos,
                             double bin0, // check if bin0 is the same as binR if yes rename it 
                             double bin1){

    std::string MatName = LayerName + "_Material";
    Acts::Transform3 base{Acts::Transform3::Identity()}; 

    container->addMaterial(MatName, [&](Acts::Experimental::MaterialDesignatorBlueprintNode* material){
      material.configureFace(Acts::CylindarVolumeBounds::Face::NegativeDisk, {Acts::AxisDirection::AxisR, Acts::AxisBoundaryType::Bound, 100}, {Acts::AxisDirection::AxisPhi, Acts::AxisBoundaryType::Bound, 100});
      material.configureFace(Acts::CylinderVolumeBounds::Face::PositiveDisc, {Acts::AxisDirection::AxisR, Acts::AxisBoundaryType::Bound, 100}, {Acts::AxisDirection::AxisPhi, Acts::AxisBoundaryType::Bound, 100});

      material.addCylindarContainer(LayerName, Acts::AxisDirection::AxisZ, [&](auto& L){
        L.addLayer(LayerName,[&](auto& layer){
          MakeLayerFunc(base*Acts::Translation3{Acts::Vector3{0, 0, z_pos}}, layer, LayerName, bin0, bin1); 
        }); 
      });
    }); 
                             }; 

void AddCylindarLayerAndMaterial(Acts::Experimental::CylinderContainerBlueprintNode* container,
                                 std::string LayerName,
                                 KdtSurfacesDim2Bin100 KdtSurfaces,
                                 double bin0,
                                 double bin1){

    std::string MatName = LayerName + "_Material";
    Acts::Transform base {Acts::Transform3::Identity()};

    container->addMaterial(MatName, [&](Acts::MaterialDesignatorBlueprintNode& mat){
                           material.configureFace(Acts::CylindarVolumeBounds::Face::OuterCylindar, {Acts::AxisDirection::AxisRPhi, Acts::AxisBoundaryType::Bound, 100}, {Acts::AxisDirection::AxisZ, Acts::AxisBoundaryType::Bound, 100});

                           material.addCylindarContainer(LayerName, Acts::AxisDirection::AxisR, [&](auto& L){
                            L.addLayer(LayerName, [&](auto& layer){
                              std::vector<std::shared_ptr<Acts::Surface>> surfaces = SelectActiveSurfaces_PhaseII(KdtSurfaces, LayerName); //I feel that it should be outside the lambda
                              
                              makeBinning(layer, Acts::SurfaceArrayNavigationPolicy::LayerType::Cylindar, bin0, bin1); 

                              //Start compiling and running ----> early debuggin is better 

                              //why is there an exception for TOB?? HOW TO BUILD THE TILTED MODULESSSS 
                              if(LayerName.find("TOB") == std::string::npos){
                                layer.setSurface(surfaces).setLayerType(Acts::Experimental::LayerBlueprintNode::LayerType::Cylinder)
                                     .setEnvelope(Acts::ExtentEnvelope{{
                                      .z = {5*Acts::UnitConstants::mm, 5*Acts::UnitConstants::mm},
                                      .r = {1*Acts::UnitConstants::mm, 1*Acts::UnitConstants::mm},
                                     }})
                                     .setTransform{base};
                                     .setUseCenterOfGravity(false, false, false); 
                              }

                            })

                            .setUseCenterOfGravity(false, false, false); 

                           });
    });
                                 }; 

//++++++++ Decorator ++++++++++++//

struct MaterialValidationCfg{
  std::size_t ntracks = 1000; 
  Acts::Vector3 startPosition = Acts::Vector3(0., 0., 0.);
  std::pair<double, double> phiRange = {-std::number::pi, std::numbers::pi};
  std::par<double, double> etaRange = {-4., 4.};

  std::shared_ptr<Acts::MaterialValidator> materialValiadator = nullptr; 

  std::string outputMaterialTracks = "material_tracks";

};

using RandomEngine = std::mt19937;
using RandomSeed = uint32_t;

//Commen the material validator and the other structs 
class MaterialValidator {
  public:
  MaterialValidator(const MaterialValidation_cfg& cfg): m_cfg(cfg){
    if(m_cfg.materialValidator == nullptr){
      throw std::invalid_argument("Missing material validator.");

    }
  }
  //Simulate a number of particles to collect the maerial info > material maps 
  std::unordered_map<std::size_t, Acts::RecordedMaterialTrack> execute(const myContext& context, bool debug = false){
    RandomSeed seed = 123456789u;
    RandomEngine rng(seed + context.EvNumber); 

    std::uniform_real_distribution<double> phyDist(m_cfg.phiRange.first, m_cfg.phiRange.second);
    std::uniform_real_distribution<double> phyDist(m_cfg.etaRange.first, m_cfg.etaRange.second);

    for(std::size_t iTrack = 0; iTrack < m_cfg.ntracks; ++iTrack){
      if(debug) std::cout << "\rEv:" << context.EvNumber << ", Track" << iTrack + 1 << "out of " << m_cfg.ntracks << std::flush; 
      
      double phi = phiDist(rng); 
      double eta = etaDist(rng);
      double theta = 2*std::atan(std::exp(-eta));
      Acts::Vector3 direction(std::cos(phi) * std::sin(theta), std::sin(phi)*std::sin(theta), std::cos(theta));

      auto rMaterial = m_cf.materialValidator->recordMaterial(
        context.geoContext, context.magFieldContext, m_cfg.startPosition,
        direction);

      recordedMaterialTracks[iTracks] = rMaterial; 
    }

    return recordedMaterialTracks; 

  }
}

struct MyMaterialWriterCfg{
  std::
} */

//DEFINE_FWK_EVENTSETUP_MODULE(Phase2GeometryProducer); */