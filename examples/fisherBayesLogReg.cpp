#include "math.h"

#include "fisherBayesLogReg.h"

fisherBayesLogReg::fisherBayesLogReg(MatrixXd& data, VectorXd& t, double alpha): 
denseFisherMetric((int)data.cols()),
mData(data),
mT(t),
mS(VectorXd::Zero((int)data.rows())),
mAlpha(alpha)
{}

double fisherBayesLogReg::V()
{
    
    mAuxVector.noalias() = mData.transpose() * mT;
    
    double v = - mQ.dot(mAuxVector);
    
    mS = mData * mQ;
    for(int n = 0; n < mS.size(); ++n)
    {
        
        if(mS(n) > 0)
        {
            v += mS(n) + log(1.0 + exp(-mS(n)));
        }
        else
        {
            v += log(1.0 + exp(mS(n)));
        }
    }
    
    v += mQ.squaredNorm() / (2.0 * mAlpha);
    
    return v;
    
}

const VectorXd& fisherBayesLogReg::gradV()
{
    
    mS = mData * mQ;
    
    double temp = 0;
    for(int n = 0; n < mS.size(); ++n)
    {
        
        if(mS(n) > 0)
        {
            mS(n) = 1.0 / ( 1.0 + exp(-mS(n)) );
        }
        else
        {
            temp = exp(mS(n));
            mS(n) = temp / (temp + 1.0);
        }
        
    }
    
    mGradV.noalias() = mData.transpose() * (-mT + mS);
    
    mGradV += (1.0 / mAlpha) * mQ;
    
    return mGradV;
    
}

void fisherBayesLogReg::fComputeG()
{
    
    mS = mData * mQ;
    
    double temp = 0;
    for(int n = 0; n < mS.size(); ++n)
    {
        
        if(mS(n) > 0)
        {
            mS(n) = 1.0 / ( 1.0 + exp(-mS(n)) );
        }
        else
        {
            temp = exp(mS(n));
            mS(n) = temp / (temp + 1.0);
        }
        
        mS(n) = mS(n) * (1.0 - mS(n));
        
    }
    
    mG.noalias() = mData.transpose() * mS.asDiagonal() * mData;
    mG += (1.0 / mAlpha) * MatrixXd::Identity(mDim, mDim);
    
}

void fisherBayesLogReg::fComputeGradG(int i)
{
    
    mS = mData * mQ;
    
    double temp = 0;
    for(int n = 0; n < mS.size(); ++n)
    {
        
        if(mS(n) > 0)
        {
            mS(n) = 1.0 / ( 1.0 + exp(-mS(n)) );
        }
        else
        {
            temp = exp(mS(n));
            mS(n) = temp / (temp + 1.0);
        }
        
        mS(n) = mS(n) * (1.0 - mS(n)) * (1.0 - 2.0 * mS(n)) * mData(n, i);
        
    }
    
    mGradG.noalias() = mData.transpose() * mS.asDiagonal() * mData;
    
}

