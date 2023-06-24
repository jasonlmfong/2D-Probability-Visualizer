#include "frameVar.h"

frameVar::frameVar()
    :m_domain_start(-5), m_domain_end(5), m_size(50), m_mode(NORMAL), m_uniform_start(-1), m_uniform_end(1), 
    m_normal_mu(0), m_normal_sigma(1), m_exponential_lambda(1),
    m_gamma_alpha(1), m_gamma_beta(1), m_beta_alpha(1), m_beta_beta(1), 
    m_chi_sq_k(1), m_T_nu(1), m_F_d1(1), m_F_d2(1)
{
}

frameVar::~frameVar()
{
}

bool frameVar::operator==(const frameVar& other)
{
    bool diff = (m_mode != other.m_mode);

    diff |= (m_size != other.m_size);

    diff |= (m_domain_start != other.m_domain_start);
    diff |= (m_domain_end != other.m_domain_end);

    diff |= (m_uniform_start != other.m_uniform_start);
    diff |= (m_uniform_end != other.m_uniform_end);

    diff |= (m_normal_mu != other.m_normal_mu);
    diff |= (m_normal_sigma != other.m_normal_sigma);

    diff |= (m_exponential_lambda != other.m_exponential_lambda);

    diff |= (m_gamma_alpha != other.m_gamma_alpha);
    diff |= (m_gamma_beta != other.m_gamma_beta);

    diff |= (m_beta_alpha != other.m_beta_alpha);
    diff |= (m_beta_beta != other.m_beta_beta);

    diff |= (m_chi_sq_k != other.m_chi_sq_k);

    diff |= (m_T_nu != other.m_T_nu);

    diff |= (m_F_d1 != other.m_F_d1);
    diff |= (m_F_d2 != other.m_F_d2);

    return !diff;
}

HeightMap frameVar::generateCorrectMap()
{
    switch (m_mode)
    {
    case UNIFORM:
        return HeightMapUniform(m_size, m_domain_start, m_domain_end, m_uniform_start, m_uniform_end);
    case NORMAL:
        return HeightMapNormal(m_size, m_domain_start, m_domain_end, m_normal_mu, m_normal_sigma);
    case EXPONENTIAL:
        return HeightMapExpo(m_size, m_domain_start, m_domain_end, m_exponential_lambda);
    case GAMMA:
        return HeightMapGamma(m_size, m_domain_start, m_domain_end, m_gamma_alpha, m_gamma_beta);
    case BETA:
        return HeightMapBeta(m_size, m_domain_start, m_domain_end, m_beta_alpha, m_beta_beta);
    case CHISQUARED:
        return HeightMapChiSq(m_size, m_domain_start, m_domain_end, m_chi_sq_k);
    case TDIST:
        return HeightMapT(m_size, m_domain_start, m_domain_end, m_T_nu);
    case FDIST:
        return HeightMapF(m_size, m_domain_start, m_domain_end, m_F_d1, m_F_d2);
    }
}
