#pragma once

#include "Mesh/HeightMap/HeightMap.h"
#include "Mesh/HeightMap/HeightMapUniform.h"
#include "Mesh/HeightMap/HeightMapNormal.h"
#include "Mesh/HeightMap/HeightMapExpo.h"
#include "Mesh/HeightMap/HeightMapGamma.h"
#include "Mesh/HeightMap/HeightMapBeta.h"
#include "Mesh/HeightMap/HeightMapChiSq.h"
#include "Mesh/HeightMap/HeightMapT.h"
#include "Mesh/HeightMap/HeightMapF.h"

enum probMode
{
    UNIFORM,
    NORMAL,
    EXPONENTIAL,
    GAMMA,
    BETA,
    CHISQUARED,
    TDIST,
    FDIST
};

class frameVar
{
public:
    frameVar();
    ~frameVar();

    bool operator==(const frameVar& other);

    HeightMap generateCorrectMap();

public:
    // value domain
    int m_domain_start;
    int m_domain_end;

    // number of vertices per side
    int m_size;

    // probability mode
    int m_mode;
    // uniform mode variables
    float m_uniform_start;
    float m_uniform_end;
    // normal mode variables
    float m_normal_mu;
    float m_normal_sigma;
    // exponential mode variables
    float m_exponential_lambda;
    // gamma mode variables
    float m_gamma_alpha;
    float m_gamma_beta;
    // beta mode variables
    float m_beta_alpha;
    float m_beta_beta;
    // chi sqaured mode variables
    int m_chi_sq_k;
    // T mode variables
    float m_T_nu;
    // F mode variables
    int m_F_d1;
    int m_F_d2;
};