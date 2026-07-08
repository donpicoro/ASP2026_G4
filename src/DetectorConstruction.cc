//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"

//MODIFY - ADD FOR THE MAGNET VOLUME
/*
#include "G4Tubs.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"
*/

#include "G4LogicalVolume.hh"
#include "G4RegionStore.hh"
#include "G4VisAttributes.hh"
#include <CLHEP/Units/SystemOfUnits.h>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    //Check overlap option
    G4bool checkOverlaps = true;

    //Materials
    G4NistManager* nist = G4NistManager::Instance();

    //MODIFY - CHANGE VACUUM TO AIR
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");

    G4Material* silicon = nist->FindOrBuildMaterial("G4_Si");

    //World
    //MODIFY - CHANGE DIMENSIONS
    G4Box* solidWorld = new G4Box("World", 0.2*CLHEP::m, 0.2*CLHEP::m, 10.*CLHEP::m);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");  
    G4VPhysicalVolume* physWorld = new G4PVPlacement
                                    (0,                    // no rotation
                                    G4ThreeVector(),       // centre position
                                    logicWorld,            // its logical volume
                                    "World",               // its name
                                    0,                     // its mother volume
                                    false,                 // no boolean operation
                                    0,                     // copy number
                                    checkOverlaps);        // overlaps checking
    logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());


    // --------------- Target ------------------------------------

    //Select target material
    //MODIFY THE MATERIAL
    G4Material* targetMaterial = nist->FindOrBuildMaterial("G4_W");

    //MODIFY - TRY TO ROTATE
    G4double angleX = 0.; //rad
    G4RotationMatrix* targetRotationMatrix = new G4RotationMatrix;
    targetRotationMatrix->rotateY(angleX);
    //MODIFY - ADD ROTATION AROUND OTHER AXES
    // targetRotationMatrix-> ...


    //Setting target dimensions
    //MODIFY THE DIMENSIONS
    G4ThreeVector targetSize = G4ThreeVector(20.*CLHEP::mm,
                                             20.*CLHEP::mm,
                                             1*CLHEP::mm);

    //Setting target position
    //MODIFY - CHANGE THE TARGET POSITION
    G4ThreeVector posTarget = G4ThreeVector(0., 0., targetSize.z()/2.);

    //target volume
    G4Box* solidTarget = new G4Box("Target",
                                    targetSize.x()/2,
                                    targetSize.y()/2,
                                    targetSize.z()/2.);
    
    G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget,
                                       targetMaterial,
                                       "Target");

    new G4PVPlacement(targetRotationMatrix,
                      posTarget,
                      logicTarget,
                      "Target",
                      logicWorld,
                      false,
                      0,
                      checkOverlaps);
    

    //visualization attributes
    //MODIFY - CHANGE COLOR OF THE TARGET
    G4VisAttributes* targetVisAttribute =
        new G4VisAttributes(G4Colour(1., 0., 0.));
    targetVisAttribute->SetForceSolid(true);
    logicTarget->SetVisAttributes(targetVisAttribute);


    // --------------- Detector -----------------------------------
    //Setting detector position
    //MODIFY - CHANGE THE DETECTOR POSITION - BE CAREFUL WITH THE WORLD DIMENSIONS
    G4ThreeVector posDetector = G4ThreeVector(0, 0, 1000*CLHEP::mm);

    //particle detector volume
    //MODIFY - CHANGE THE DETECTOR DIMENSIONS
    G4Box* detector = new G4Box("Detector",
                                10*CLHEP::cm/2,
                                10*CLHEP::cm/2,
                                0.3*CLHEP::mm/2);
    
    //MODIFY - PUT ANOTHER MATERIAL
    G4LogicalVolume* logicDetector = new G4LogicalVolume(detector,
                                                         silicon,
                                                         "Detector");
    new G4PVPlacement(0, 
                      posDetector, 
                      logicDetector,
                      "Detector", 
                      logicWorld, 
                      false, 
                      0, 
                      checkOverlaps);

    //visualization attributes
    //MODIFY - CHANGE COLOR OF THE DETECTOR
    G4VisAttributes* detectorVisAttribute =
        new G4VisAttributes(G4Colour(0., 0., 1));
    detectorVisAttribute->SetForceSolid(true);
    logicDetector->SetVisAttributes(detectorVisAttribute);

//  MODIFY - ADD THE DIPOLE MAGNET VOLUME; STUDY THE CODE, TRY TO CHANGE
    /*
    G4double innerRadius = 0.*CLHEP::cm;
    G4double outerRadius = 30.*CLHEP::cm;
    G4double height = 25.*CLHEP::cm; // half-height
    G4double startAngle = 0.*CLHEP::deg;
    G4double spanningAngle = 360.*CLHEP::deg;

    G4ThreeVector posMagnet = G4ThreeVector(0., 0., 0.5*CLHEP::m);

    G4Tubs* magnet = new G4Tubs("Magnet",innerRadius,outerRadius,height,startAngle,spanningAngle);
    G4RotationMatrix *xRot = new G4RotationMatrix;
    xRot->rotateX(90.*CLHEP::deg);
    G4LogicalVolume* logicMagnet = new G4LogicalVolume(magnet,world_mat,"Magnet");
    new G4PVPlacement(xRot,posMagnet,logicMagnet,"Magnet",logicWorld,false,0,checkOverlaps);

    //visualization attributes
    G4VisAttributes* magnetVisAttribute =
        new G4VisAttributes(G4Colour(0., 1., 1, 0.3));//MODIFY - CHANGE OPACITY
                                                      //(HINT - COMPARE WITH THE PREVIOUS CASE)
    magnetVisAttribute->SetForceSolid(true);
    logicMagnet->SetVisAttributes(magnetVisAttribute);
    */
    
    //always return the physical World
    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    //MODIFY - ADD THE MAGNET FIELD; STUDY THE CODE
    /*
    G4double fieldValue = 0.5*CLHEP::tesla; //MODIFY THE MAGNETIC FIELD VALUE
    G4UniformMagField* myField = new G4UniformMagField(G4ThreeVector(0., fieldValue, 0.));
    G4FieldManager* fieldMgr =
        G4TransportationManager::GetTransportationManager()->GetFieldManager();
    G4LogicalVolume* logicBox1 = G4LogicalVolumeStore::GetInstance()->GetVolume("Magnet");
    G4FieldManager* localfieldMgr = new G4FieldManager(myField);
    logicBox1->SetFieldManager(localfieldMgr,true);
    fieldMgr->CreateChordFinder(myField);
    */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

