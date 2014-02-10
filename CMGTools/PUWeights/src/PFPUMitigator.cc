// -*- C++ -*-
//
// Package:    PFNeutralPUWeighter
// Class:      PFNeutralPUWeighter
// 
/**\class PFNeutralPUWeighter PFNeutralPUWeighter.cc CMSDAS/PFNeutralPUWeighter/src/PFNeutralPUWeighter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Michail Bachtis,40 1-B08,+41227678176,
//         Created:  Mon Dec  9 13:18:05 CET 2013
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"

//
// class declaration
//

class PFPUMitigator : public edm::EDProducer {
   public:
      explicit PFPUMitigator(const edm::ParameterSet&);
      ~PFPUMitigator();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      virtual void beginRun(edm::Run&, edm::EventSetup const&);
      virtual void endRun(edm::Run&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

      // ----------member data ---------------------------
  


};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
PFPUMitigator::PFPUMitigator(const edm::ParameterSet& iConfig)
{
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
 
   //if you want to put into the Run
   produces<ExampleData2,InRun>();
*/
   //now do what ever other initialization is needed


  produces<reco::PFCandidateCollection>();
  
}


PFPUMitigator::~PFPUMitigator()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
PFPUMitigator::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   edm::Handle<reco::PFCandidateCollection> pfCharged;
   edm::Handle<reco::PFCandidateCollection> pfNeutral;
   edm::Handle<reco::PFCandidateCollection> pfPU;

   iEvent.getByLabel("pfAllChargedParticles",pfCharged);
   iEvent.getByLabel("pfAllNeutralHadronsAndPhotons",pfNeutral);
   iEvent.getByLabel("pfPileUpIso",pfPU);

   double sumNPU;
   double sumPU;

   std::auto_ptr<reco::PFCandidateCollection> out(new reco::PFCandidateCollection); 


   for (unsigned int i=0;i<pfNeutral->size();++i) {
     sumNPU=0.0;
     sumPU=0.0;
     for (unsigned int j=0;j<pfCharged->size();++j) {
       sumNPU += 1./deltaR2(pfNeutral->at(i).eta(),
			    pfNeutral->at(i).phi(),
			    pfCharged->at(j).eta(),
			    pfCharged->at(j).phi());
     }
     for (unsigned int j=0;j<pfPU->size();++j) {
       sumPU += 1./deltaR2(pfNeutral->at(i).eta(),
			    pfNeutral->at(i).phi(),
			    pfPU->at(j).eta(),
			    pfPU->at(j).phi());
     }

     reco::PFCandidate neutral = pfNeutral->at(i);
     //     neutral.set_mva_e_pi((sumNPU/pfCharged->size()-sumPU/pfPU->size())/(sumNPU/pfCharged->size()+sumPU/pfPU->size()));
     double weight =  (sumNPU)/(sumNPU+sumPU);
     neutral.setP4(neutral.p4()*weight);
     out->push_back(neutral);
     
   }
   for (unsigned int i=0;i<pfCharged->size();++i) {
     out->push_back(pfCharged->at(i));
     
     
   }
   iEvent.put(out);
 
}

// ------------ method called once each job just before starting event loop  ------------
void 
PFPUMitigator::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PFPUMitigator::endJob() {
}

// ------------ method called when starting to processes a run  ------------
void 
PFPUMitigator::beginRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
PFPUMitigator::endRun(edm::Run&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
PFPUMitigator::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
PFPUMitigator::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
PFPUMitigator::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(PFPUMitigator);
