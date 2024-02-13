//
// Created by robot1 on 10/9/23.
//

#include <iostream>
#include "Estimation.h"


Estimation::Estimation(MatrixXd xt, MatrixXd Pt) {
    reset(xt,Pt);
}

void Estimation::reset(MatrixXd xt, MatrixXd Pt) {
    this->xt =  xt;
    //printMatrix(&xt,"xt");
    this->Pt =  Pt;
    //printMatrix(&Pt,"Pt");
}

void Estimation::update(Matching * matching,const MatrixXd* xEst,const MatrixXd* pEst) {
    MatrixXd Rt = matching->getMatches()->getRt();
    const MatrixXd Ht = matching->getMatches()->getHt();
    const MatrixXd vt = matching->getMatches()->getVt();

    //printMatrix(xEst,"--xEst--");
    //printMatrix(&Rt,"--Rt--");
    //printMatrix(&Ht,"--Ht--");
    //printMatrix(pEst,"--Pest--");
    //printMatrix(&vt,"--vt--");
    //cout << "Rt size " <<  Rt.rows() << " x " << Rt.cols() << endl;
    //cout << "Ht size " <<  Ht.rows() << " x " << Ht.cols() << endl;
    //cout << "vt size " <<  vt.rows() << " x " << vt.cols() << endl;

    MatrixXd PtIN = Ht * *pEst * Ht.transpose() + Rt;
    //printMatrix(&PtIN,"--PtIN--");
    //printMatrix(pEst,"--PtEst--");
    MatrixXd Kt = *pEst * Ht.transpose() * PtIN.inverse();
    //printMatrix(&Kt,"--Kt--");
    //printMatrix(&vt,"--vt--");
    ktGain = Kt * vt;
    xt = *xEst + ktGain;
    Pt = *pEst - Kt * PtIN * Kt.transpose();
    //printMatrix(&xt,"--xt--");
    //printMatrix(&Pt,"--Pt--");
    //cout << "x " <<  xt(0,0) << " y " << xt(1,0) << " theta " << xt(2,0) << endl;
    /*
    xt_est = validated{i}.xt_est;
    Pt_est = validated{i}.Pt_est;
    PtIN  = obj.Ht * Pt_est * obj.Ht' + obj.Rt;
    MatrixXd Kt     = Pt_est*obj.Ht'*(PtIN)^(-1);
    xt     = xt_est + Kt*(obj.zt - obj.zt_est);
    Pt     = Pt_est - Kt * PtIN * Kt';
    */
    printDebugData(matching);
}


const MatrixXd * Estimation::getXt() const {
    return &xt;
}

const MatrixXd * Estimation::getPt() const {
    return &Pt;
}

double Estimation::getX() {
    return getXt()->coeff(0,0);
}

double Estimation::getY() {
    return getXt()->coeff(1,0);
}

double Estimation::getTheta() {
    return getXt()->coeff(2,0);
}

void Estimation::printDebugData(Matching * matching) {
    //MatrixXd vtUnStacked = matching->getMatches()->getVtUnstacked();
    //printMatrix(&vtUnStacked,"---- Vt ----");
    cout << getX() << " " << getY() << " " << getTheta();
    //<< " " << ktGain.coeff(0,0) << " " << ktGain.coeff(1,0) <<" " << ktGain.coeff(2,0);
    cout << endl;
}




