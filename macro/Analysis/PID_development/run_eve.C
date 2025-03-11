
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

std::vector<const AtHit *> xpfit(const std::vector<const AtHit *> &hits)
{
   std::vector<const AtHit *> hits2;
   for (auto hit : hits)
      hits2.push_back(hit);

   auto sort_func = [](const AtHit *A, const AtHit *B) { return (*A).GetPosition().Z() > (*B).GetPosition().Z(); };
   std::sort(hits2.begin(), hits2.end(), sort_func);

   // define the percent you want fitted
   double xpercent = hits.size() * 0.35;
   int minpoints = 75;
   if (xpercent < minpoints)
      xpercent = minpoints;

   // convert the float to an integer
   int maxHits = static_cast<int>(xpercent);
   if (hits.size() < maxHits)
      maxHits = hits.size();

   // Copy all hits to the new vector and return that vector.
   std::vector<const AtHit *> hitsToFit;
   for (int i = 0; i < maxHits; i++) { // for i in range(maxHits)
      hitsToFit.push_back(hits2[i]);
      std::cout << "Including hit " << i << " at rho = " << hits2.at(i)->GetPosition().Rho()
                << " and z = " << hits2.at(i)->GetPosition().Z() << std::endl;
   }

   return hitsToFit;
}

// Function definition for selecting which hits to include in a fit.
std::vector<const AtHit *> radfit(const std::vector<const AtHit *> &hits)
{

   std::vector<const AtHit *> hits2;
   for (auto hit : hits)
      hits2.push_back(hit);

   auto sort_func = [](const AtHit *A, const AtHit *B) { return (*A).GetPosition().Z() > (*B).GetPosition().Z(); };
   std::sort(hits2.begin(), hits2.end(), sort_func);

   // define your radius 'r' in mm
   double radius = 200.0;
   int maxHits = 0;
   int minpoints = 75;
   for (int i = 0; i < hits.size(); i++) {
      double x = hits2.at(i)->GetPosition().X();
      double y = hits2.at(i)->GetPosition().Y();
      double distr = (x * x) + (y * y);
      if (distr < radius * radius) {
         ++maxHits;

      } else
         break;
   }
   if (maxHits < minpoints) {
      maxHits = minpoints;
   }
   if (hits.size() < maxHits)
      maxHits = hits.size();

   // Copy all hits to the new vector and return that vector.
   std::vector<const AtHit *> hitsToFit;
   for (int i = 0; i < maxHits; i++) {
      hitsToFit.push_back(hits2.at(i));
      std::cout << "Including hit " << i << " at rho = " << hits2.at(i)->GetPosition().Rho()
                << " and z = " << hits2.at(i)->GetPosition().Z() << std::endl;
   }


   return hitsToFit;
}

// Function definition for selecting which hits to include in a fit.
std::vector<const AtHit *> pointtrackfit(const std::vector<const AtHit *> &hits)
{

   std::vector<const AtHit *> hits2;
   for (auto hit : hits)
      hits2.push_back(hit);

   auto sort_func = [](const AtHit *A, const AtHit *B) { return (*A).GetPosition().Z() > (*B).GetPosition().Z(); };
   std::sort(hits2.begin(), hits2.end(), sort_func);

   // define your radius 'r' in mm
   double radius = 200.0;
   int maxHits = 0;
   int minpoints = 75;
   
   // pick the first point as the circle's center
   double xCenter = hits2[0]->GetPosition().X();
   double yCenter = hits2[0]->GetPosition().Y();

   for (int i = 0; i < hits.size(); i++) {

      //Reuse x & y from rad fit
      double x = hits2.at(i)->GetPosition().X();
      double y = hits2.at(i)->GetPosition().Y();

      // Shift by the first point of the track (new center)
      double dx = x - xCenter;
      double dy = y - yCenter;

      double distr = (dx * dx) + (dy * dy);
      if (distr < radius * radius) {
         ++maxHits;

      } else
         break;
   }
   if (maxHits < minpoints) {
      maxHits = minpoints;
   }
   if (hits.size() < maxHits)
      maxHits = hits.size();

   // Copy all hits to the new vector and return that vector.
   std::vector<const AtHit *> hitsToFit;
   for (int i = 0; i < maxHits; i++) {
      hitsToFit.push_back(hits2.at(i));
      std::cout << "Including hit " << i << " at rho = " << hits2.at(i)->GetPosition().Rho()
                << " and z = " << hits2.at(i)->GetPosition().Z() << std::endl;
   }


   return hitsToFit;
}

void run_eve(TString InputDataFileName = "run_0053")
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
   pra->SetPruneHitsForFit(radfit);

   AtPRAtask *praTask = new AtPRAtask(pra);
   praTask->SetPersistence(false); // Do not save the output of this task
   praTask->SetMaxNumHits(3000);
   praTask->SetMinNumHits(200);
   praTask->SetTcluster(8.0);
   praTask->SetOutputBranch("radfit");

   // Create tasks for performing additional analysis
   AtPATTERN::AtTrackFinderTC *pra2 = new AtPATTERN::AtTrackFinderTC();
   pra2->SetTcluster(8);
   pra2->SetMaxHits(3000);
   pra2->SetMinHits(200);
   pra2->SetPruneHitsForFit(xpfit);

   AtPRAtask *praTask2 = new AtPRAtask(pra2);
   praTask2->SetPersistence(false); // Do not save the output of this task
   praTask2->SetMaxNumHits(3000);
   praTask2->SetMinNumHits(200);
   praTask2->SetTcluster(8.0);
   praTask2->SetOutputBranch("xpfit");

   // Create tasks for performing additional analysis
   AtPATTERN::AtTrackFinderTC *pra3 = new AtPATTERN::AtTrackFinderTC();
   pra3->SetTcluster(8);
   pra3->SetMaxHits(3000);
   pra3->SetMinHits(200);
   pra3->SetPruneHitsForFit(pointtrackfit);
   
   AtPRAtask *praTask3 = new AtPRAtask(pra3);
   praTask3->SetPersistence(false); // Do not save the output of this task
   praTask3->SetMaxNumHits(3000);
   praTask3->SetMinNumHits(200);
   praTask3->SetTcluster(8.0);
   praTask3->SetOutputBranch("pointradfit");


   AtViewerManager *eveMan = new AtViewerManager(fMap);

   auto tabMain = std::make_unique<AtTabMain>();
   tabMain->SetMultiHit(100); // Set the maximum number of multihits in the visualization
   tabMain->SetDrawProjection(true);
   eveMan->AddTab(std::move(tabMain));

   eveMan->AddTask(praTask);
   eveMan->AddTask(praTask2);
   eveMan->AddTask(praTask3);


   eveMan->Init();
}
