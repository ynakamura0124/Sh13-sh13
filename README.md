# CloverSim

## requirements
 - yaml-cpp (https://github.com/jbeder/yaml-cpp)
 - Geant4.10 (It doesn't work with 4.11.)
 - ROOT >6.14
 
## installation
```
git clone https://github.com/rin-yokoyama/cloversim.git
cd cloversim
mkdir build
cd build
cmake .. (-DCMAKE_INSTALL_PREFIX=[path_to_yaml-cpp_if_not_found])
make; make install
```

## usage
```
source thiscloversim.sh
cloversim -c config.yaml
```

## config.yaml
```
RunConfigurator:
        DetectorConstruction: [Name of the EventAction to load]
        PhysicsList: [Name of the PhysicsList to load]
        PrimaryGeneratorAction: [Name of the PrimaryGeneratorAction to load]
        RunAction: [Name of the RunAction to load]
        EventAction: [Name of the EventAction to load]
        
CloverSimAnalysisManager:
        FileName: [output ROOT file name]
        FileOption: [TF1 option ('RECREATE'/'UPDATE'/'NEW')]

PointGammaSourceGeneratorAction:
        SourceFile: [file of gamma source definition]
        NEvent: [Number of Decays to simulate]
        PositionX: [x position of the point source in mm]
        PositionY: [y position of the point source in mm]
        PositionZ: [z position of the point source in mm]

ImplantedGammaSourceGeneratorAction:
        TreeName: [Input tree name]
        GammaEnergy: [Gamma energy in keV]
        BranchName: [Name of branch for implantation event]
        PIDGate: [True/False]
        # Parameters for an ellipse to cut pid: A(X-X0)^2 - B(U-Y0)^2 < 1
        RadA: 18000 # Ga84
        RadB: 2.5
        X0: 2.71050
        Y0: 31.10
        MergerList: #List of input rootfiles to chain
                - FileName: [ROOT file 1]
                - FileName: [ROOT file 2]
```
PointGammaSourceAction emits gamma-rays from a point source. It simulates cascade decays from the source as you defined in the SourceFile.
ImplantedGammaSourceGeneratorAction emits fixed energy gamma-rays from the position distribution given in the experimental ROOT files. requires -DCLOVERSIM_USE_BRIKEN=ON.
