//
// Created by mpolovyi on 28/01/16.
//

#ifndef PROJECT_EXTENSIONDEFS_H
#define PROJECT_EXTENSIONDEFS_H

int get_next(int pt, int ptCount, int step = 1);

constexpr double GetRadius() {
    return 0.607592984 / 2.0;
}

constexpr double GetInertia() {
    return 2 * 1 * GetRadius() * GetRadius() / 5.0;
}

int get_nearest_index(double* arr, double val, int size);

extern "C" void Function_GetCorrelations(char ** input_string,
                                         int*_ptCount,
                                         int* _corrCount,
                                         double* _maxCorrLength,
                                         double* correlations_out,
                                         int* corr_counts_out,
                                         double* corr_lengths_out,
                                         double* _systemSize);

extern "C" void Function_GetChainOrientationProbabilityCorrelation(char ** input_string,
                                                                   int*_ptCount,
                                                                   double*_correlationCutOff,
                                                                   int* corr_counts_out,
                                                                   double* corr_lengths_out,
                                                                   double* _systemSize);

extern "C" int Function_ChangeBinaryToBaseParticles(void * input_string, void * output_string, int ptCount);

extern "C" void Function_GetParticleOrientationProbability(char ** input_string,
                                                           int*_ptCount,
                                                           int* corr_counts_out);

extern "C" void Function_GetChainOrientationProbabilityEnergy(char ** input_string,
                                                              int*_ptCount,
                                                              double*  _separationCutOff,
                                                              int* corr_counts_out,
                                                              int* corr_counts_out_2,
                                                              double* corr_lengths_out,
                                                              double* _systemSize);

extern "C" void Function_GetChainOrientationProbabilityAngle(int*breaks_counts,
                                                             int*chain_counts,
                                                             char ** input_string,
                                                             int*_ptCount,
                                                             double*  _angleCutOff, double* _distanceCutOff);

extern "C" void Function_GetDynamicChains(double *neigh_c,
                                          double *coords_first,
                                          int *chained,
                                          char **pts,
                                          int *_strLength,
                                          int *_timePointsCount,
                                          int *_ptCount,
                                          double *_systemSize);

extern "C" void Function_GetChainOrientationProbability(char ** input_string,
                                                        int*_ptCount,
                                                        double*  _separationCutOff,
                                                        int* corr_counts_out,
                                                        double* corr_lengths_out,
                                                        double* _systemSize);

extern "C" void Function_GetChainOrientationProbabilityTest(char ** input_string,
                                                            int*_ptCount,
                                                            double*  _separationCutOff,
                                                            int* corr_counts_out,
                                                            double* _systemSize);

extern "C" void Function_UnwrapBinary(char ** input_string, int* in_size, double * ret);

extern "C" void Function_AutoCorrelation(double * averAutoCorr, int* sampleIndex, char ** zero_configuration, char ** current_configuration, int * ptCount);

extern "C" void Function_AutoCorrelationInCluster(double * averAutoCorr,
                                                  char ** zero_configuration,
                                                  char ** current_configuration,
                                                  int * _ptCount,
                                                  int * _blocksCount);

#endif //PROJECT_EXTENSIONDEFS_H
