#include "SiteResponse.h"

#include <iostream>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>




// these must be defined here!!
StandardStream sserr;
FileStream ferr("fem.log");
OPS_Stream *opserrPtr = &ferr;
OPS_Stream *opsoutPtr = &sserr;


SiteResponse::SiteResponse(std::string configureFile,std::string anaDir,std::string outDir,std::string femLog, std::function<void(double)> callbackFunction ) :
    m_configureFile(configureFile),
    m_analysisDir(anaDir),
    m_outputDir(outDir),
    m_femLog(femLog)
{
    m_callbackFunction = callbackFunction;
    init(configureFile,anaDir,outDir);
    model->setCallback(true);
}

SiteResponse::SiteResponse(std::string configureFile,std::string anaDir,std::string outDir,std::string femLog) :
    m_configureFile(configureFile),
    m_analysisDir(anaDir),
    m_outputDir(outDir),
    m_femLog(femLog)
{
    init(configureFile,anaDir,outDir);
}


void SiteResponse::init(std::string configureFile,std::string anaDir,std::string outDir)
{

    // set fem log
    ferr_true = new FileStream(m_femLog.c_str());
    opserrPtr = ferr_true;

    //./siteresponse ../test/siteLayering.loc -bbp ../test/9130326.nwhp.vel.bbp out thisLog
    // read the layering file
    std::string layersFN("/Users/simcenter/Codes/SimCenter/SiteResponseTool/test/siteLayering.loc");
    std::string bbpOName(".");
    SiteLayering siteLayers(layersFN.c_str());
    int t = siteLayers.getNumLayers();


    int inputStyle = 2; // bbp=1 opensees=2

    if (inputStyle == 1)
    {
        std::string bbpFName("/Users/simcenter/Codes/SimCenter/SiteResponseTool/test/9130326.nwhp.vel.bbp");
        bbpOName = std::string("out");
        std::string bbpLName = std::string("thisLog");
        //ferr.setFile(bbpLName.c_str(), APPEND);
        // read bbp style motion
        motionX.setBBPMotion(bbpFName.c_str(), 1);
        //motionZ.setBBPMotion(bbpFName.c_str(), 2);

        model = new SiteResponseModel(siteLayers, "2D", &motionX);
        model->setOutputDir(bbpOName);
    }
    else {
        std::string bbpLName = "Log";
        //std::string motionXFN("/Users/simcenter/Codes/SimCenter/SiteResponseTool/test/RSN766_G02_000_VEL");
        std::string motionXFN(anaDir+"/Rock-x");//TODO: may not work on windows
        motionX.setMotion(motionXFN.c_str());
        bbpOName = "out";
        model = new SiteResponseModel("2D", &motionX, m_callbackFunction);
        //model = new SiteResponseModel("2D", &motionX);
        model->setOutputDir(outDir);
        model->setAnalysisDir(anaDir);
        model->setTclOutputDir(outDir);
        model->setConfigFile(configureFile);


        //buildTcl();
    }

}
void SiteResponse::buildTcl()
{
    bool runAnalysis = false;
    model->buildEffectiveStressModel2D(runAnalysis);
}

void SiteResponse::buildTcl3D()
{
    bool runAnalysis = false;
    model->buildEffectiveStressModel3D(runAnalysis);
}

int SiteResponse::run()
{
    bool runAnalysis = true;
    if (model->buildEffectiveStressModel2D(runAnalysis)!=100)
        return -1;
    else
        return 1;
}


SiteResponse::~SiteResponse()
{

}

