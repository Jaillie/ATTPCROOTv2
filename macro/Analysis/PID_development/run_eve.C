
// Function definition for selecting which hits to include in a fit.
std::vector<const AtHit *> SelectHitsForFit(const std::vector<const AtHit *> &hits)
{
   // Example function to only take the first 50 hits
   int maxHits = 50;
   if (hits.size() < maxHits)
      maxHits = hits.size();

   // Copy all hits to the new vector and return that vector.
   std::vector<const AtHit *> hitsToFit;
   for (int i = 0; i < maxHits; i++) {
      hitsToFit.push_back(hits.at(i));
      std::cout << "Including hit " << i << " at rho = " << hits.at(i)->GetPosition().Rho()
                << " and z = " << hits.at(i)->GetPosition().Z() << std::endl;
   }

   return hitsToFit;
}

void run_eve(TString InputDataFileName = "run_0052")
{

   TString InputDataFile = "data/" + InputDataFileName + ".root";

   TString OutputDataFile = "data/" + InputDataFileName + ".reco_display.root";

   TString dir = getenv("VMCWORKDIR");
   TString geoFile = "ATTPC_He1bar_v2_geomanager.root";
   TString mapFile = "e12014_pad_mapping.xml";
   TString parameterFile = "ATTPC.e20020.par";

   TString InputDataPath = InputDataFile;
   TString OutputDataPath = OutputDataFile;
   TString GeoDataPath = dir + "/geometry/" + geoFile;
   TString mapDir = dir + "/scripts/" + mapFile;

   FairRunAna *fRun = new FairRunAna();
   FairRootFileSink *sink = new FairRootFileSink(OutputDataFile);
   FairFileSource *source = new FairFileSource(InputDataFile);
   fRun->SetSource(source);
   fRun->SetSink(sink);
   fRun->SetGeomFile(GeoDataPath);

   FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
   FairParRootFileIo *parIo1 = new FairParRootFileIo();
   // parIo1->open("param.dummy.root");
   rtdb->setFirstInput(parIo1);

   auto fMap = std::make_shared<AtTpcMap>();
   fMap->ParseXMLMap(mapDir.Data());

   // Create tasks for performing additional analysis
   AtPATTERN::AtTrackFinderTC *pra = new AtPATTERN::AtTrackFinderTC();
   pra->SetTcluster(8);
   pra->SetMaxHits(3000);
   pra->SetMinHits(200);
   pra->SetPruneHitsForFit(SelectHitsForFit);

   AtPRAtask *praTask = new AtPRAtask(pra);
   praTask->SetPersistence(false); // Do not save the output of this task
   praTask->SetMaxNumHits(3000);
   praTask->SetMinNumHits(200);
   praTask->SetTcluster(8.0);
   praTask->SetOutputBranch("PrunedTracks");

   AtViewerManager *eveMan = new AtViewerManager(fMap);

   auto tabMain = std::make_unique<AtTabMain>();
   tabMain->SetMultiHit(100); // Set the maximum number of multihits in the visualization
   tabMain->SetDrawProjection(true);
   eveMan->AddTab(std::move(tabMain));

   eveMan->AddTask(praTask);

   eveMan->Init();
}
