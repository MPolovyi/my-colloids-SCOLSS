//
// Created by mpolovyi on 28/01/16.
//

#include "ExtensionDefs.h"
// Created by mpolovyi on 28/01/16.
//

#include <iostream>
#include <cmath>
#include "math.h"

#include <SCOLSS/ParticlePhysics/CParticleBase.h>
#include <SCOLSS/ParticlePhysics/CYukawaDipolePt.h>

#include "cereal/archives/json.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"

std::vector<CParticleBase> LoadParticles(std::string pString, size_t PtCount) {
    std::stringstream in_stream;

    in_stream << "{\"value0\": \"";
    in_stream << pString;
    in_stream << "\"}";
    cereal::JSONInputArchive arch(in_stream);
    std::vector<CParticleBase> particles;
    particles.resize(PtCount);
    arch.loadBinaryValue(&particles[0], sizeof(CParticleBase) * PtCount);

    return particles;
}

int get_next(int pt, int ptCount, int step) {
    pt += step;
    while (pt >= ptCount)
        pt -= ptCount;

    return pt;
}

int get_nearest_index(double* arr, double val, int size){
    int ret = 0;
    double dst_old = 100000;
    for(int i = 0; i < size; i++){
        double dst = std::abs(val - arr[i]);
        if (dst < dst_old) {
            dst_old = dst;
            ret = i;
        }
    }
    return ret;
}

extern "C" void Function_GetCorrelations(char ** input_string,
                                         int*_ptCount,
                                         int* _corrCount,
                                         double* _maxCorrLength,
                                         double* correlations_out,
                                         int* corr_counts_out,
                                         double* corr_lengths_out,
                                         double* _systemSize) {
    int ptCount = _ptCount[0];
    double& maxCorrLength = _maxCorrLength[0];
    int& corrCount = _corrCount[0];
    double& systemSize = _systemSize[0];

    std::string input(*input_string);
    std::stringstream in_stream;

    in_stream << "{\"value0\": \"";
    in_stream << input;
    in_stream << "\"}";
    cereal::JSONInputArchive arch(in_stream);
    std::vector<CParticleBase> particles;

    for (int i = 0; i < ptCount; ++i) {
        particles.push_back(CParticleBase());
    }
    arch.loadBinaryValue(&particles[0], sizeof(CParticleBase) * ptCount);

    double dist = 0;
    for (int i = 0; i < ptCount; i++) {
        auto& pt = particles[i];

        int j = get_next(i, ptCount);

        auto cosTheta1 = pt.GetOrientation().Z;

        correlations_out[0] += cosTheta1*cosTheta1;
        corr_counts_out[0]++;
        while (j != i) {
            const auto& pt_next = particles[j];

            dist = pt.GetDistanceRight(pt_next, systemSize).GetLength();
            if(dist < maxCorrLength) {
                auto nIndex = get_nearest_index(corr_lengths_out, dist, corrCount);

                auto cosTheta2 = pt_next.GetOrientation().Z;

                correlations_out[nIndex] += cosTheta1 * cosTheta2;
                corr_counts_out[nIndex]++;

                j = get_next(j, ptCount);
            }
            else{
                break;
            }
        }
    }
}

class CWrongQuaternion {
public:
    CVector V;
    double W;
};
class CWrongPtBase {
public:
    double Coordinates;
    CWrongQuaternion Rotation;
};

extern "C" int Function_ChangeBinaryToBaseParticles(void * input_string, void * output_string, int ptCount) {
    std::string input((char*)input_string);
    std::stringstream in_stream;

    in_stream << "{\"value0\": \"";
    in_stream << input;
    in_stream << "\"}";

//    std::cout << in_stream.str() << std::endl;

    cereal::JSONInputArchive arch(in_stream);
    std::vector<CWrongPtBase> particles;
    for (int i = 0; i < ptCount; ++i) {
        particles.push_back(CWrongPtBase());
    }
    arch.loadBinaryValue(&particles[0], sizeof(CWrongPtBase) * ptCount);

    std::vector<CParticleBase> saveParticles;
    for (int i = 0; i < ptCount; ++i){
        auto pt = CParticleBase();
        pt.Coordinates = particles[i].Coordinates;
        CQuaternion newOrient;
        newOrient.W = particles[i].Rotation.W;
        newOrient.V = particles[i].Rotation.V;
        pt.SetRotation(CQuaternion(newOrient));
        saveParticles.push_back(pt);
    }

    std::stringstream out_stream;

    cereal::JSONOutputArchive oarch(out_stream);
    oarch.saveBinaryValue(&saveParticles[0], sizeof(CParticleBase)*ptCount);

    std::string out_string = out_stream.str();
    for(int i = 17; i < out_string.length()-1; i++) {
        ((char*)output_string)[i-17] = out_string[i];
    }

    return strlen((char*)output_string);
}

extern "C" void Function_GetParticleOrientationProbability(char ** input_string,
                                                           int*_ptCount,
                                                           int* corr_counts_out) {
    int ptCount = _ptCount[0];

    std::string input(*input_string);
    std::stringstream in_stream;

    in_stream << "{\"value0\": \"";
    in_stream << input;
    in_stream << "\"}";
    cereal::JSONInputArchive arch(in_stream);
    std::vector<CParticleBase> particles;

    for (int i = 0; i < ptCount; ++i) {
        particles.push_back(CParticleBase());
    }

    arch.loadBinaryValue(&particles[0], sizeof(CParticleBase) * ptCount);

    double dist = 0;
    for (int i = 0; i < ptCount; i++) {
        auto& pt = particles[i];

        auto cosTheta1 = pt.GetOrientation().Z;

        if(cosTheta1 > 0) {
            corr_counts_out[1]++;
        }
        else {
            corr_counts_out[0]++;
        }
    }
}

extern "C" void Function_GetChainOrientationProbabilityEnergy(char ** input_string,
                                                        int*_ptCount,
                                                        double*  _separationCutOff,
                                                        int* corr_counts_out,
                                                        double* corr_lengths_out,
                                                        double* _systemSize) {
    int ptCount = _ptCount[0];
    double separationCutOff = _separationCutOff[0];
    double systemSize = _systemSize[0];

    std::string input(*input_string);
    std::stringstream in_stream;

    in_stream << "{\"value0\": \"";
    in_stream << input;
    in_stream << "\"}";
    cereal::JSONInputArchive arch(in_stream);
    std::vector<CParticleBase> particles;

    for (int i = 0; i < ptCount; ++i) {
        particles.push_back(CParticleBase());
    }
    arch.loadBinaryValue(&particles[0], sizeof(CParticleBase) * ptCount);

    bool chain_1;
    bool chain_2;

    int chainLength = 0;

    for (int i = 0; i < particles.size(); i++) {
        auto& pt = particles[i];
        chain_1 = pt.GetOrientation().Z > 0;
        auto& pt_next = particles[get_next(i, ptCount)];

        auto cosTheta = pt.GetOrientation().Z;
        auto cosTheta_next = pt_next.GetOrientation().Z;

        chain_2 = cosTheta_next > 0;

        CYukawaDipolePt pt1 = CYukawaDipolePt(1000, 10, systemSize);
        CYukawaDipolePt pt2 = CYukawaDipolePt(1000, 10, systemSize);

        pt1.SetRotation(pt.GetRotation());
        pt2.SetRotation(pt_next.GetRotation());

        pt1.Coordinates = pt.Coordinates;
        pt2.Coordinates = pt_next.Coordinates;

        double energy = pt1.GetPotentialEnergy(pt2, pt1.GetDistanceRight(pt2, systemSize));

        if(energy < separationCutOff) {
            chainLength++;
        }
        else {
            // rr, lr, rl, ll
            if (chain_1 && chain_2) {
                corr_counts_out[0]++;
                corr_lengths_out[0] += pt.GetDistanceRight(pt_next, systemSize).GetLength();

                corr_lengths_out[4] += energy;
            }
            if (!chain_1 && chain_2) {
                corr_counts_out[1]++;
                corr_lengths_out[1] += pt.GetDistanceRight(pt_next, systemSize).GetLength();

                corr_lengths_out[5] += energy;
            }
            if (chain_1 && !chain_2) {
                corr_counts_out[2]++;
                corr_lengths_out[2] += pt.GetDistanceRight(pt_next, systemSize).GetLength();

                corr_lengths_out[6] += energy;
            }
            if (!chain_1 && !chain_2) {
                corr_counts_out[3]++;
                corr_lengths_out[3] += pt.GetDistanceRight(pt_next, systemSize).GetLength();

                corr_lengths_out[7] += energy;
            }

            if (chain_1) {
                corr_counts_out[4]++;
            }
            else {
                corr_counts_out[5]++;
            }

            corr_counts_out[6] += chainLength;
            chainLength = 0;
        }
    }
}

extern "C" void Function_GetChainOrientationProbability(char ** input_string,
                                                        int*_ptCount,
                                                        double*  _separationCutOff,
                                                        int* corr_counts_out,
                                                        double* corr_lengths_out,
                                                        double* _systemSize) {
    int ptCount = _ptCount[0];
    double separationCutOff = _separationCutOff[0];
    double systemSize = _systemSize[0];

    std::string input(*input_string);
    std::stringstream in_stream;

    in_stream << "{\"value0\": \"";
    in_stream << input;
    in_stream << "\"}";
    cereal::JSONInputArchive arch(in_stream);
    std::vector<CParticleBase> particles;

    for (int i = 0; i < ptCount; ++i) {
        particles.push_back(CParticleBase());
    }
    arch.loadBinaryValue(&particles[0], sizeof(CParticleBase) * ptCount);

    bool chain_1;
    bool chain_2;

    int chainLength = 0;

    for (int i = 0; i < particles.size(); i++) {
        auto& pt = particles[i];
        chain_1 = pt.GetOrientation().Z > 0;
        auto& pt_next = particles[get_next(i, ptCount)];

        auto cosTheta = pt.GetOrientation().Z;
        auto cosTheta_next = pt_next.GetOrientation().Z;

        chain_2 = cosTheta_next > 0;

        if((pt.GetDistanceRight(pt_next, systemSize).GetLength() <= separationCutOff) && (cosTheta * cosTheta_next >= 0)){
            chainLength++;
        }
        else {
            // rr, lr, rl, ll
            CYukawaDipolePt pt1 = CYukawaDipolePt(1000, 10, systemSize);
            CYukawaDipolePt pt2 = CYukawaDipolePt(1000, 10, systemSize);

            pt1.SetRotation(pt.GetRotation());
            pt2.SetRotation(pt_next.GetRotation());

            pt1.Coordinates = pt.Coordinates;
            pt2.Coordinates = pt_next.Coordinates;

            if (chain_1 && chain_2) {
                corr_counts_out[0]++;
                corr_lengths_out[0] += pt.GetDistanceRight(pt_next, systemSize).GetLength();

                corr_lengths_out[4] += pt1.GetPotentialEnergy(pt2, pt1.GetDistanceRight(pt2, systemSize));
            }
            if (!chain_1 && chain_2) {
                corr_counts_out[1]++;
                corr_lengths_out[1] += pt.GetDistanceRight(pt_next, systemSize).GetLength();

                corr_lengths_out[5] += pt1.GetPotentialEnergy(pt2, pt1.GetDistanceRight(pt2, systemSize));
            }
            if (chain_1 && !chain_2) {
                corr_counts_out[2]++;
                corr_lengths_out[2] += pt.GetDistanceRight(pt_next, systemSize).GetLength();

                corr_lengths_out[6] += pt1.GetPotentialEnergy(pt2, pt1.GetDistanceRight(pt2, systemSize));
            }
            if (!chain_1 && !chain_2) {
                corr_counts_out[3]++;
                corr_lengths_out[3] += pt.GetDistanceRight(pt_next, systemSize).GetLength();

                corr_lengths_out[7] += pt1.GetPotentialEnergy(pt2, pt1.GetDistanceRight(pt2, systemSize));
            }

            if (chain_1) {
                corr_counts_out[4]++;
            }
            else {
                corr_counts_out[5]++;
            }

            corr_counts_out[6] += chainLength;
            chainLength = 0;
        }
    }
}

extern "C" void Function_GetChainOrientationProbabilityTest(char ** input_string,
                                                            int*_ptCount,
                                                            double*  _separationCutOff,
                                                            int* corr_counts_out,
                                                            double* _systemSize) {
    int ptCount = _ptCount[0];
    double separationCutOff = _separationCutOff[0];
    double systemSize = _systemSize[0];

    std::string input(*input_string);
    std::stringstream in_stream;

    in_stream << "{\"value0\": \"";
    in_stream << input;
    in_stream << "\"}";
    cereal::JSONInputArchive arch(in_stream);
    std::vector<CParticleBase> particles;

    for (int i = 0; i < ptCount; ++i) {
        particles.push_back(CParticleBase());
    }
    arch.loadBinaryValue(&particles[0], sizeof(CParticleBase) * ptCount);

    particles = std::vector<CParticleBase>();

    double displ = 0;
    for(int i = 0; i < 10; i++){
        auto pt = CParticleBase();
        pt.Coordinates = i*0.9 + displ;

        particles.push_back(pt);
    }

//    displ = 12;
//    for(int i = 0; i < 10; i++){
//        auto pt = CParticleBase();
//        pt.Coordinates = i*0.9 + displ;
//
//        particles.push_back(pt);
//    }
//
//    displ = 24;
//    for(int i = 0; i < 10; i++){
//        auto pt = CParticleBase();
//        pt.Coordinates = i*0.9 + displ;
//        pt.Rotation = CQuaternion(M_PI, CVector(0, 1, 0));
//        particles.push_back(pt);
//    }
//
//
    displ = 36;
    for(int i = 0; i < 10; i++){
        auto pt = CParticleBase();
        pt.Coordinates = i*0.9 + displ;
        pt.SetRotation(CQuaternion(M_PI, CVector(0, 1, 0)));
        particles.push_back(pt);
    }

    displ = 48;
    for(int i = 0; i < 10; i++){
        auto pt = CParticleBase();
        pt.Coordinates = i*0.9 + displ;
        pt.SetRotation(CQuaternion(M_PI, CVector(0, 1, 0)));
        particles.push_back(pt);
    }


    bool chain_1 = particles[0].GetOrientation().Z > 0;
    bool chain_2 = true;

    int chainLength = 0;

    for (int i = 0; i < particles.size(); i++) {
        auto& pt = particles[i];
        auto& pt_next = particles[get_next(i, ptCount)];

        auto cosTheta = pt.GetOrientation().Z;
        auto cosTheta_next = pt_next.GetOrientation().Z;

        chain_2 = cosTheta_next > 0;

        if(pt.GetDistanceRight(pt_next, systemSize).GetLength() <= separationCutOff && cosTheta * cosTheta_next >= 0){
            chainLength++;
        }
        else {
            // ll, rl, lr, rr
            if (chain_1 && chain_2) corr_counts_out[3]++;
            if (!chain_1 && chain_2) corr_counts_out[2]++;
            if (chain_1 && !chain_2) corr_counts_out[1]++;
            if (!chain_1 && !chain_2) corr_counts_out[0]++;

            if (chain_1) {
                corr_counts_out[4]++;
            }
            else {
                corr_counts_out[5]++;
            }

            corr_counts_out[6] += chainLength;
            chainLength = 0;

            chain_1 = chain_2;
        }
    }
}

extern "C" void Function_UnwrapBinary(char ** input_string, int* in_size, double * ret) {
    int size = in_size[0];

    std::string input(*input_string);
    std::stringstream in_stream;

    in_stream << "{\"value0\": \"";
    in_stream << input;
    in_stream << "\"}";

    cereal::JSONInputArchive arch(in_stream);

    arch.loadBinaryValue(ret, sizeof(double) * size);
}


extern "C" void Function_GetDynamicChains(double* neigh_c,
                                          double* coords_first,
                                          char** pts,
                                          int* _strLength,
                                          int* _timePointsCount,
                                          int* _ptCount,
                                          double* _systemSize) {
    int PtCount = _ptCount[0];
    double SystemSize = _systemSize[0];
    int ptStringLength = _strLength[0];

    std::vector<std::vector<CParticleBase>> particles;

    std::string pts_i(ptStringLength, 0);

    for (int k = 0; k < _timePointsCount[0]; ++k) {
        strncpy(&pts_i[0], &pts[0][k * ptStringLength], ptStringLength);

        particles.push_back(LoadParticles(&pts_i[0], PtCount));
    }

    for (int i = 0; i < PtCount; ++i) {
        int next_index = get_next(i, PtCount);

        double dot = 0;
        CVector vec_i;
        CVector vec_j;

        for (int j = 0; j < _timePointsCount[0]; ++j) {
            dot += particles[j][i].GetOrientation().DotProduct(particles[j][next_index].GetOrientation());

            vec_i += particles[j][i].GetOrientation();
            vec_j += particles[j][next_index].GetOrientation();
        }

        neigh_c[i] = dot/3.0 - (vec_i/3).DotProduct((vec_j/3));

        coords_first[i] = particles[0][i].Coordinates;
    }
}