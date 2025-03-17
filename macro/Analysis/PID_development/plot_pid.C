
void GetEnergy(Double_t M, Double_t IZ, Double_t BRO, Double_t &E);

void plot_pid()
{
   /*** Open the unpacked file and the corresponding TTree ***/
   FairRunAna *run = new FairRunAna();
   TString FileName = "./data/run_0053.root"; // File to open and read the data from
   std::cout << " Opening File : " << FileName.Data() << std::endl;
   TFile *file = new TFile(FileName.Data(), "READ");

   TTree *tree = (TTree *)file->Get("cbmsim");
   Int_t nEvents = tree->GetEntries();
   std::cout << " Number of events : " << nEvents << std::endl;

   /** The only branch we care about is the one that has our pattern**/
   TTreeReader Reader1("cbmsim", file);
   TTreeReaderValue<TClonesArray> eventArray(Reader1, "AtPatternEvent");

   TH2F *bro_vs_eloss = new TH2F("bro_vs_eloss", "bro_vs_eloss", 4000, 0, 25000.0, 500, 0, 3);
   TH2F *bro_vs_dedx = new TH2F("bro_vs_dedx", "bro_vs_dedx", 4000, 0, 4000.0, 500, 0, 3);

   int i = 0;
   while (Reader1.Next()) {
      if (i++ % 100 == 0)
         std::cout << " Event Number : " << i << "\n";

      AtPatternEvent *patternEvent = (AtPatternEvent *)eventArray->At(0);
      if (patternEvent == nullptr)
         continue;

      std::vector<AtTrack> &patternTrackCand = patternEvent->GetTrackCand();
      std::cout << " Number of pattern tracks " << patternTrackCand.size() << "\n";
      for (auto track : patternTrackCand) {

         Double_t theta = track.GetGeoTheta();
         Double_t rad = track.GetGeoRadius();

         Double_t B_f = 3.0; // Magnitude of the magnetic field in Tesla

         double bro = B_f * rad / TMath::Sin(theta) / 1000.0; // Brho in Tm

         // PID - copied from C15_dd_ana.C (experiment a2091)
         Double_t len = 0;
         Double_t eloss = 0;
         Double_t dedx = 0;

         // Energy loss from ADC
         auto hitClusterArray = track.GetHitClusterArray();
         std::size_t cnt = 0;
         Double_t zpos = 0;

         // If the track is forward
         if (theta * TMath::RadToDeg() < 90) {
            auto firstCluster = hitClusterArray->back();
            zpos = firstCluster.GetPosition().Z();
            auto it = hitClusterArray->rbegin();
            while (it != hitClusterArray->rend()) {

               if (((Float_t)cnt / (Float_t)hitClusterArray->size()) > 0.7)
                  break;
               auto dir = (*it).GetPosition() - (*std::next(it, 1)).GetPosition();
               eloss += (*it).GetCharge();
               len += std::sqrt(dir.Mag2());
               dedx += (*it).GetCharge();
               // std::cout<<(*it).GetCharge()<<"\n";
               it++;
               ++cnt;
            }
         } else if (theta * TMath::RadToDeg() > 90) { // If the track is backward

            auto firstCluster = hitClusterArray->front();
            zpos = firstCluster.GetPosition().Z();
            eloss += hitClusterArray->at(0).GetCharge();

            cnt = 1;
            for (auto iHitClus = 1; iHitClus < hitClusterArray->size(); ++iHitClus) {

               if (((Float_t)cnt / (Float_t)hitClusterArray->size()) > 0.7)
                  break;
               auto dir = hitClusterArray->at(iHitClus).GetPosition() - hitClusterArray->at(iHitClus - 1).GetPosition();
               len += std::sqrt(dir.Mag2());
               eloss += hitClusterArray->at(iHitClus).GetCharge();
               dedx += hitClusterArray->at(iHitClus).GetCharge();
               // std::cout<<len<<" - "<<eloss<<" - "<<hitClusterArray->at(iHitClus).GetCharge()<<"\n";
               ++cnt;
            }
         }

         eloss /= cnt;
         dedx /= len;

         bro_vs_eloss->Fill(eloss, bro);
         bro_vs_dedx->Fill(dedx, bro);
      }
   } // End of the event loop
}