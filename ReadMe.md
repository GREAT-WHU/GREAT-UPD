---------------------------------------------------------------------------------------------

              GREAT-UPD: An Open-source Software for GNSS UPD Estimation

---------------------------------------------------------------------------------------------

OVERVIEW

GREAT-UPD is an open-source software for multi-GNSS (global navigation satellite system) and 
multi-frequency UPD (uncalibrated phase delay) estimation, which is one of the important 
modules in GREAT (GNSS+ REsearch, Application and Teaching) software designed and developed at 
Wuhan University. GREAT-UPD consists of several portable program libraries and an application 
program utilizing these libraries. The features of GREAT-UPD are:

(1) It supports multi-GNSS and multi-frequency UPD estimation for: 

    GPS, GLONASS, Galileo, BeiDou
    
(2) All executable binary APs (application programs) for Windows, Linux, Macintosh are included 
    in the package, as well as the whole source programs of the libraries and AP.

(3) The open-source, cross-platform compilation tool CMake is adopted, which is convenient for 
    users to customize their own executable APs.

(4) Cycle slip detection tool, efficient batch processing python scripts [Download 
    GNSS Data, Cycle slip Detection, UPD Estimation] & plotting and analysis scripts of UPD are 
    provided. 

---------------------------------------------------------------------------------------------

DIRECTORY STRUCTURE OF PACKAGE

  GREAT-UPD_<ver>
  ./bin                              the executable binary APs for Windows/Linux/Macintosh *
  ./src                              source programs of GREAT-UPD libraries *
    ./app                            main function of UPD estimation *
    ./LibUPD                         source programs of UPD estimation library *
    ./LibMat                         source programs of the newmat library *
    ./LibGnut                        source programs of the G-Nut library *
  ./sample data                      sample data for AP *
    ./UPD_2020001                    sample data for GREAT-UPD AP *
  ./util                             utilities *
    ./batch_process                  batch processing python scripts *
    ./PreEdit                        cycle slip detection tool & sample data *
    ./upd_analysis                   plotting and analysis scripts of UPD *
  ./doc                              document files *
    ./UPD_config                     sample XML files for GREAT-UPD *
	GREAT-UPD_1.0.pdf                user manual

---------------------------------------------------------------------------------------------

INSTALLATION AND USAGE

 1. Get CMake via the website (https://cmake.org/download/) and install it. Note that the 
    minimum requirement of CMake version is 3.0.0.

 2. Download GREAT-UPD from https://geodesy.noaa.gov/gps-toolbox
 
 3. Extract the program package GREAT-UPD_<ver>.zip or GREAT-UPD_<ver>.tgz to appropriate
    directory <install_dir>. 

    > gzip -d GREAT-UPD_<ver>.zip or tar xzf GREAT-UPD_<ver>.tgz (Linux)
                   
 4. Run the cmake executable or the cmake-gui to configure the project and then build
    it with your chosen build tool. 
    
    For Linux/Macintosh: 
    Creat "build" directory in the directory <install_dir>/GREAT-UPD_<ver>
    and then run the following commands to compile the source code. The executable AP can
    be found in <install_dir>/GREAT-UPD_<ver>/build/Bin.
    
        > mkdir <install_dir>/GREAT-UPD_<ver>/build
        > cd <install_dir>/GREAT-UPD_<ver>/build
        > cmake ../
        > make
    
    For Windows: 
    Drag CMakeList.txt in the directory <install_dir>/GREAT-UPD_<ver> to the
    interface of CMake-gui and then perform the following operations.
        > Modify attribute "Where to build the binaries" as <install_dir>/GREAT-UPD_<ver>/build
        > Click "Configure" button.
        > Choose the Integrated Development Environment (IDE) for the project. (appears only
          the first time you click "Configure" button)
        > Click "Generate" button.
        > Click "Open Project" button and compile source code.
    
 5. Run the program (in GREAT-UPD_<ver>/build/Bin) with XML config

    > <install_dir>/GREAT-UPD_<ver>/build/Bin/<pgm> -x CONFIG.xml

---------------------------------------------------------------------------------------------

MANUAL

  Refer GREAT-UPD_<ver>/doc/GREAT-UPD_<ver>.pdf.

---------------------------------------------------------------------------------------------

CONTRIBUTOR

   Core developers:

   * Xingxing Li (xxli@sgg.whu.edu.cn)
   * Xin Li (lixinsgg@whu.edu.cn)
   * Guolong Feng (glfeng@whu.edu.cn)
   * Xinjuan Han (xjhansgg@whu.edu.cn)
   * Gege Liu 
   * Jiaxin Huang 
   * Bo Wang 


   Third party libraries:

   * GREAT-UPD uses G-Nut library (http://www.pecny.cz).
     G-Nut is Copyright (C) 2011-2016 GOP - Geodetic Observatory Pecny, RIGTC.
     
   * GREAT-UPD uses pugixml library (http://pugixml.org).
     pugixml is Copyright (C) 2006-2014 Arseny Kapoulkine.

   * GREAT-UPD uses Newmat library (http://www.robertnz.net/nm_intro.htm)
     Newmat is  Copyright (C) 2008: R B Davies.

---------------------------------------------------------------------------------------------

HISTORY

  ver.1.0      2020/08/05 new release

---------------------------------------------------------------------------------------------

COPYRIGHT AND LICENSE

          Copyright (C) 2018-2020, Wuhan University. All rights reserved.

 Licensed under the GNU General Public License Version 3.
 See LICENSE details.

---------------------------------------------------------------------------------------------

