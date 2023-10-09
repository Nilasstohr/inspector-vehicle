//
// Created by robot1 on 10/9/23.
//

#include "Estimation.h"

Estimation::Estimation() {
    xt = new MatrixXd(3,1);
    Pt = new MatrixXd(3,3);
}

void Estimation::update(Matches *matches) {
    printMatrix(matches->)
    /*
    xt_est = validated{i}.xt_est;
    Pt_est = validated{i}.Pt_est;
    Pt_IN  = obj.Ht * Pt_est * obj.Ht' + obj.Rt;
    MatrixXd Kt     = Pt_est*obj.Ht'*(Pt_IN)^(-1);
    xt     = xt_est + Kt*(obj.zt - obj.zt_est);
    Pt     = Pt_est - Kt * Pt_IN * Kt';
    */
 }


