/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#include <iostream>
#include <iomanip>
#include <limits>
#define BOOST_TEST_MODULE sp_algo_nn

#include <boost/test/unit_test.hpp>
#include "sp/algo/nn/layer/fully_connected.hpp"
#include "sp/algo/nn/network.hpp"
#include "assert_matrix.hpp"
#include "sp/algo/nn/gradient_check.hpp"

#include "sp/util/typename.hpp" // @TODO REMOVE

using namespace sp::algo::nn;
using namespace sp::testing;

BOOST_AUTO_TEST_CASE(test_fully_connected_fprop, * boost::unit_test::disabled()) {

    using layer_type = fully_connected_layer<volume_dims<1, 1, 4>, 2>;
    layer_type fc;
    fc.weight_initializer = fixed_weight_initializer(1.0);
    fc.bias_initializer = fixed_weight_initializer(0.5);

    fc.configure(1, true);

    tensor_4 in(1,1,1,4);
    in.setValues({{{{0, 1, 2, 3}}}});

    tensor_4 out(1,2,1,1); out.setZero();

    tensor_4 expected(1,2,1,1);
    expected.setValues({{{{6.5}},{{6.5}}}});

    fc.forward_prop(in, out);

    assert_tensor_equals(expected, out);

}

BOOST_AUTO_TEST_CASE(test_fully_connected_fprop_2) {

    /** From MNIST */

    using layer_type = fully_connected_layer<volume_dims<120>, 10ul>;
    layer_type fc;

    fc.weight_initializer = vector_weight_initializer({-0.207657,-0.19552,0.0216335,-0.213925,0.197698,-0.0912099,0.211262,0.0354688,0.1121,-0.148856,-0.0869734,-0.0843074,-0.178743,0.00794865,0.123318,-0.14536,-0.0962762,-0.00902826,0.0808702,0.205989,0.0246527,0.203638,-0.0338434,-0.0517365,0.108364,-0.0781954,0.183674,0.145404,0.11457,-0.0194826,0.0529469,-0.0998569,-0.168214,-0.004631,-0.0756407,-0.0541692,0.0596225,-0.213692,0.15996,-0.0302566,0.00548488,-0.161051,-0.0989542,-0.167539,0.196583,0.204344,-0.16888,0.0876709,-0.080946,0.00757305,0.091878,-0.0542523,-0.190091,0.032345,0.0166099,-0.0905211,0.114171,-0.159634,-0.159179,0.174601,-0.0945792,0.0676559,0.153326,-0.193098,0.212912,0.141388,-0.0757582,0.0643821,-0.0209083,0.0184451,-0.0689774,-0.0853092,-0.0713454,0.161567,0.178799,-0.105036,0.0358655,-0.0179995,-0.153207,-0.142606,0.0280354,-0.186276,0.150629,-0.0068136,0.0841007,0.0976445,-0.0356836,0.0734646,0.194217,0.10762,0.174144,0.0528994,0.106094,-0.0794193,0.15319,0.0145144,0.00160936,-0.196696,-0.132977,-0.0479896,0.086923,-0.0105621,0.063627,-0.213495,-0.0790276,-0.171746,-0.0512977,-0.121755,0.085107,-0.0213559,-0.197364,0.141306,0.106458,0.0879737,0.120628,-0.0766959,0.150196,0.137711,0.195096,-0.142793,-0.147108,0.0447018,0.100008,-0.0270302,-0.0909191,0.0522783,-0.06388,0.144116,0.16985,-0.193824,0.0352662,-0.139857,0.0494676,0.0485244,-0.0407847,0.0891835,-0.159833,0.164869,0.125879,-0.024782,0.1133,-0.0191434,-0.0685949,-0.145487,0.134212,-0.194239,-0.161472,-0.0464355,0.196926,0.192973,-0.17949,-0.136445,0.071989,-0.195103,-0.0441904,0.033932,0.148055,0.141962,-0.195275,-0.0273836,-0.195669,0.041488,-0.130422,0.0651967,-0.198286,-0.0841276,-0.0864006,-0.0414623,-0.0736809,0.0486019,-0.187221,-0.0286529,-0.0940022,0.0311651,-0.0128809,0.0348994,0.00786112,0.016139,0.126749,-0.209083,0.0661901,-0.0468701,0.0933863,-0.129547,0.163203,0.0198341,-0.0903826,-0.148555,0.0166148,-0.0895487,0.112632,0.0741318,-0.107888,0.0902267,-0.212274,-0.135411,-0.0243501,-0.131552,-0.0642072,-0.0892512,0.0283444,-0.155143,-0.169807,0.133752,0.0124389,0.11135,0.21024,0.172926,0.207628,-0.0588616,-0.0217602,-0.0781727,-0.124004,-0.121851,-0.124858,0.176919,0.0363321,0.0658603,-0.164758,0.0652293,-0.112752,-0.0052724,0.166542,0.120415,0.20651,-0.128627,-0.0763296,-0.0996821,0.0690957,0.160684,0.0795372,0.154608,0.115742,-0.0323035,0.213805,0.0595037,0.0676302,0.0726561,0.0737597,0.0901365,-0.185215,-0.16331,0.209671,0.0931369,-0.043671,0.210593,0.0543427,-0.169554,-0.175466,-0.143442,0.185181,0.134955,-0.0903645,-0.042041,-0.102181,0.196321,0.087753,0.0852725,0.149656,-0.0232441,-0.116552,-0.176133,0.167404,-0.117256,0.162663,-0.0215893,0.185456,-0.0872263,0.159192,-0.0301399,0.178348,0.156644,-0.0146338,0.101194,-0.209271,0.016487,-0.191004,0.0240318,-0.0781713,-0.0203336,0.107497,-0.194524,0.174348,-0.0606674,0.153186,-0.194651,0.146413,-0.00086318,0.00298785,0.127608,0.0189334,-0.0263593,-0.139704,-0.0814199,-0.124584,-0.116883,-0.115126,-0.0542201,0.0591913,-0.0498322,0.0967544,-0.184263,-0.210683,-0.0103025,-0.152019,-0.157334,0.121231,0.0991229,0.0925701,0.111413,-0.078772,-0.0039214,0.177626,0.178059,-0.0891784,0.0130677,-0.0406093,0.0664763,-0.0831798,-0.177566,0.114329,-0.163718,-0.0893073,-0.210165,0.0461953,-0.0765955,-0.154555,0.0171246,0.00124753,0.0477056,0.0326493,-0.144313,0.0561765,0.153856,-0.0907973,0.0102065,-0.0888855,0.208829,0.104701,-0.0385998,-0.101467,-0.113965,-0.171347,0.189428,-0.100346,-0.0881319,-0.0862326,-0.0646144,0.174237,0.100638,-0.10578,-0.0305399,0.164926,0.0689094,-0.207074,0.11319,-0.209563,0.122796,0.00900751,0.16786,0.0916511,-0.0655828,-0.0785207,-0.0199642,-0.185032,0.131419,0.0741851,0.157781,-0.0130939,-0.146057,-0.0355246,-0.0562817,0.149443,-0.011315,-0.141196,-0.00715549,0.214303,-0.161238,0.0488079,0.178283,-0.0650051,0.12477,-0.0865609,0.131513,-0.0211186,-0.0715982,0.0954835,-0.067504,-0.14265,0.136883,-0.0583427,0.147255,-0.128389,-0.0196717,-0.165034,0.109777,-0.211017,-0.105591,-0.0547188,0.129892,-0.000694916,0.169541,-0.206031,0.214448,-0.0366531,0.114471,0.127891,-0.145946,0.130978,0.0296365,0.129305,-0.175767,0.132164,0.0928075,0.174809,0.0206725,-0.0763956,0.0410279,-0.206993,0.19536,-0.2018,0.129085,-0.0918823,0.0538725,0.048906,-0.209995,-0.115233,-0.117019,0.1362,0.0510294,0.00779071,0.00254294,-0.212032,-0.00343674,0.0790162,0.033065,-0.146023,-0.100137,-0.0987013,0.0719015,-0.0542661,-0.10801,0.0373386,0.159093,-0.0660791,0.0330087,-0.175257,0.192363,-0.209577,-0.193034,0.129154,0.0466468,-0.203729,0.0717854,-0.177636,-0.164258,0.144201,0.212554,0.0456468,-0.0969484,-0.193419,-0.052287,0.0521575,-0.0191681,-0.0081351,0.120407,0.171682,0.0924094,-0.103798,-0.0316334,-0.0303878,-0.103187,-0.0871356,-0.174829,-0.193895,-0.129458,0.0306156,0.111069,-0.0801154,-0.18941,0.11371,0.112877,0.193999,0.0495068,-0.0013501,-0.149664,-0.0674024,0.123486,-0.172384,-0.0872488,-0.105738,0.0762132,0.0487956,-0.0897182,0.0231475,0.052199,-0.12449,-0.083392,0.0664507,-0.130833,0.0624995,-0.190594,-0.115641,0.0526801,-0.111768,-0.128094,-0.20954,-0.214391,0.0975948,-0.0717387,0.123255,0.00702818,-0.19185,-0.0519516,-0.0807405,0.0624648,-0.111117,-0.0666861,-0.0723402,-0.108887,-0.116344,-0.0431665,0.0386043,-0.171204,0.135379,0.054788,-0.0711753,-0.21205,0.207043,0.164707,-0.111071,0.0373481,0.125953,0.0216148,-0.189597,-0.110049,0.185797,-0.189857,0.0800636,0.0127338,-0.0141499,-0.168261,0.0620911,0.0817427,-0.0998898,-0.210977,0.161174,0.0255914,0.118048,-0.0393168,0.167004,0.132103,0.148537,0.106593,-0.151888,0.191485,0.130413,-0.0920806,-0.177829,-0.0232497,-0.20369,-0.0634341,-0.0717914,0.0548314,-0.0169156,0.133733,-0.147089,-0.0116173,0.174479,0.0641443,-0.0731547,0.0425637,0.134785,-0.125845,-0.0522034,0.0415028,-0.0054235,0.125438,0.167306,-0.101385,-0.079815,-0.0459483,-0.120414,-0.169746,0.18949,0.0788313,-0.00525382,-0.0544169,-0.191017,-0.178797,0.151327,0.145919,0.143732,-0.104382,-0.04216,0.192419,0.0297473,0.0430768,-0.124735,0.0327028,-0.173776,0.153319,0.00979045,0.0792854,-0.147343,0.0623367,0.0317068,0.142694,0.101298,-0.0897233,-0.0236057,0.207823,0.049189,0.0162393,0.213743,-0.148168,0.178306,0.209708,-0.182469,-0.137532,-0.167963,-0.13655,-0.168681,-0.102749,0.208268,-0.0611422,0.108216,-0.0495197,-0.0359949,-0.125471,0.0174176,0.0359341,-0.0880075,0.142426,-0.208762,-0.186221,-0.178893,0.0400343,-0.0534441,-0.187094,-0.0433754,-0.0224465,0.0593196,0.214031,0.0142214,0.129665,-0.0182183,-0.0504553,0.150568,-0.0396314,-0.165427,-0.158611,0.0212315,-0.0706122,-0.0576669,0.101593,0.204657,0.155037,0.168496,0.158351,-0.132063,-0.186658,0.161932,0.190565,-0.160745,-0.16949,0.0469252,0.0226558,-0.18273,0.0338111,-0.180211,0.156692,0.127998,0.182311,0.043509,0.0321469,-0.183343,0.185073,0.0683703,-0.031986,0.159687,0.0145647,-0.19366,0.050255,-0.0817103,0.0403613,-0.0841657,0.00906931,-0.1803,0.0927775,0.158149,0.0641936,-0.0559907,0.0070295,-0.129908,0.0282146,0.172834,-0.129786,0.168997,0.13711,0.0753946,0.157885,-0.058375,0.199965,-0.0500682,0.20826,0.211095,0.141162,-0.112751,-0.150508,-0.0490416,0.132462,-0.0314278,-0.124177,0.13759,0.0379369,-0.13336,-0.16576,-0.0299335,0.17789,0.183982,0.0337552,-0.198034,-0.0162222,-0.0556655,-0.0776897,-0.166295,-0.033535,-0.122844,-0.147337,0.129787,0.147503,-0.145107,0.0634331,-0.110421,-0.0186327,-0.195832,-0.11414,0.126789,0.0318068,-0.130765,0.129785,0.145492,0.151271,0.0726263,-0.0926764,0.0408516,0.203069,-0.00992875,0.0320049,-0.196631,0.0694775,-0.10661,0.152349,-0.0166892,-0.0596488,-0.0991206,0.151067,0.213423,-0.105014,-0.177249,0.00127129,-0.0714891,-0.172008,0.180408,-0.0694902,-0.0171822,-0.0214369,0.137967,0.165254,0.045866,0.0937035,-0.0177115,0.201708,0.0153756,0.0488004,0.100077,0.160342,-0.0208548,0.0427871,0.0585444,0.151741,-0.103184,0.175046,-0.111358,-0.207765,0.0280228,0.213691,-0.0791279,0.20662,-0.195398,-0.0225635,0.00898805,-0.213454,-0.10372,-0.0560436,-0.061126,0.202663,0.153578,0.105337,0.0807151,0.161196,0.212177,0.159239,0.200572,-0.0831415,0.064823,-0.0640999,-0.208207,-0.102948,-0.184553,-0.110337,-0.0643929,0.0974049,-0.101829,0.0514923,-0.127819,0.0673876,-0.0188505,0.127171,0.11073,0.20355,0.171154,-0.0542965,-0.159474,-0.0664867,0.0298771,0.0178542,-0.173166,0.0588585,-0.0817113,0.127331,-0.103913,-0.0138481,0.170848,0.170524,-0.113577,0.196125,-0.0780855,0.0866868,0.118869,0.211087,-0.162208,-0.0364143,0.00738139,0.193715,-0.0649409,0.179359,0.147365,0.0492505,0.192382,0.143937,0.106403,-0.190349,0.0329815,-0.207546,0.00811335,-0.0690553,0.0912195,0.00179106,0.0825781,0.115581,-0.0659336,0.0246652,0.109358,-0.140159,-0.000720963,-0.139408,0.212683,-0.040891,-0.121173,0.0475585,0.0978291,-0.0173053,0.174116,0.024807,0.158132,-0.0202183,0.0615388,0.0865571,-0.189748,0.145701,-0.0343215,0.163256,-0.063601,-0.170826,0.192023,-0.0828647,-0.164532,0.184225,0.0392692,-0.126055,-0.118573,0.0691621,0.0775677,-0.0755816,-0.0855014,-0.196145,0.0372053,0.0338758,0.00340556,-0.200339,-0.0694712,-0.0910638,0.15022,-0.00355442,0.0261604,-0.12528,0.11125,-0.0538192,0.0230709,-0.0428749,0.00765565,-0.0479823,-0.102308,-0.0716937,-0.0880926,0.0677711,0.0552207,0.0967243,-0.0685175,-0.136968,-0.191592,0.0505384,-0.212578,-0.0464927,-0.0108565,-0.0375374,0.0689407,0.0712337,-0.175094,0.148181,0.131104,-0.208792,0.0900351,-0.126984,0.140769,-0.0716427,-0.154098,-0.136307,-0.02525,0.0590835,-0.14122,0.0725338,0.199622,-0.144499,-0.207629,0.0782043,-0.141315,-0.162704,0.078861,0.0932447,0.0835287,0.0481719,-0.213029,-0.119601,-0.154797,-0.173558,-0.161589,-0.0245073,-0.143977,-0.100242,-0.0802456,0.0651905,0.077148,0.000553161,0.0291686,-0.0529202,-0.179849,-0.213146,-0.10993,0.0453606,-0.169041,-0.123906,-0.169037,0.153435,0.185896,0.117113,-0.0316038,-0.0397256,-0.0573122,0.0122433,0.151814,-0.117143,-0.155429,-0.0378074,-0.061497,0.0818905,0.207685,-0.0131823,0.194024,-0.148052,-0.0881954,-0.195756,-0.185619,-0.149521,-0.0935073,0.166623,0.152748,0.198521,0.155171,-0.0589323,-0.115448,0.176372,0.199893,-0.0497858,-0.146279,0.16314,0.0353866,-0.142295,0.112259,0.0559263,0.103358,0.107593,-0.128148,-0.0816418,0.0931551,0.0786485,0.148608,0.180779,-0.166983,0.125876,0.0962569,-0.0622786,0.024265,-0.0651864,-0.20416,-0.00513771,0.198081,-0.159994,0.000976235,-0.0852452,-0.198433,0.12439,-0.0379893,-0.134839,0.00393261,-0.198448,0.192462,0.0974491,-0.00889295,0.0176407,0.173567,-0.110215,-0.158144,-0.0237677,0.0496683,0.0988585,0.141321,0.060123,0.0304859,0.202169,0.0632205,-0.0195976,-0.179304,0.168799,-0.123724,0.0128622,0.173295,0.1742,-0.189931,-0.112875,0.21087,-0.107837,0.144767,0.0296242,0.1843,0.189163,-0.198897,-0.0506658,-0.152851,-0.0769462,-0.110084,-0.188818,0.137609,-0.0685104,0.16675,0.0767769,0.101608,0.00440513,0.0835303,0.0399706,-0.0520337,0.0453346,0.129548,0.0429631,-0.144909,-0.169505,0.0269777,0.135325,0.0650653,0.0217768,-0.0961928,-0.162452,-0.163778,0.0832517,-0.0416591,0.0575419,-0.0426382,0.153095,0.173868,-0.163866,-0.103605,0.109142,0.0740395,-0.0339799,0.15698,0.0131722,-0.204627,0.0898201,-0.0568064,0.150563,0.124209,-0.0299588,-0.0760838,-0.167475,-0.181808,-0.0288663,-0.016242,0.0746169,0.0864951,-0.0733794,-0.158653,0.207825,-0.133634,0.125625,0.118621,0.159272,0.152523,0.0630021,0.140206,-0.101429,0.00688429,0.110214,-0.121161,-0.186113,0.0400169,-0.173957,-0.0620222,0.0289969,-0.00254843,0.100808,-0.0264383,-0.0736092,0.141114,0.122283,0.0306284,-0.0301826,-0.0848867,-0.125752,-0.0549433,-0.0664768,-0.0617653,-0.130227,-0.138512,-0.191898,-0.0218166,0.212014,0.197255,-0.158931,-0.00984928,0.0615893,0.00590347,0.135709,-0.039789,0.145046,-0.114177,0.191896,0.0803621,0.058887,-0.172523,0.120987,0.0682924,0.211369,-0.0426024,-0.152255,0.0324557,0.0119286,0.156269,0.090706,0.049265,-0.055906,-0.0490992,-0.111481,-0.0819186,0.154139,-0.0831231,0.178344,0.048715,0.151026,0.201346,-0.0400832,0.0339956,0.17282,0.13527,-0.152255,-0.114726,-0.198146,0.0337453,0.077376});
    fc.bias_initializer = vector_weight_initializer({0.211292,0.118792,-0.171657,-0.0284915,-0.0701956,-0.0490734,-0.0244068,-0.0440654,0.0986503,0.211617});

    fc.configure(1, true);

    tensor_4 in(1, 120, 1, 1);
    in.setValues({{{{0.100495, -0.0740347, 0.00847659, 0.0457791, -0.0224482, -0.03484, -0.0179062, -0.0699442, -0.0562099, 0.0712692, -0.119404, 0.0482172, 0.201278, -0.0737239, 0.0510407, 0.0305444, -0.0616385, -0.0400481, 0.0892127, 0.116911, 0.0932202, -0.152367, -0.0376082, 0.0413685, -0.00791901, -0.0781655, -0.0452816, 0.0615268, 0.0186277, 0.111809, -0.0458763, -0.0660881, -0.0036263, 0.0981885, 0.122594, -0.0305555, 0.141635, 0.0792892, 0.0472425, 0.0612026, 0.159881, 0.0100733, 0.00119554, -0.0698559, -0.0188613, 0.0648741, 0.0111808, 0.0876926, 0.00574239, 0.0857317, -0.0433582, 0.0846826, -0.0952492, 0.0490915, -0.105617, 0.122994, 0.0348597, -0.14265, -0.0133145, -0.0620206, 0.0539471, 0.0399651, 0.00309656, -0.0856518, -0.0132899, -0.0457192, 0.0802512, -0.147274, 0.029216, 0.023331, 0.040885, -0.0195623, 0.0498875, 0.0581005, -0.0781479, -0.0298167, -0.0934772, -0.0176526, 0.118385, 0.0345887, 0.0316182, 0.00524014, 0.02734, -0.0364829, 0.031119, 0.107636, 0.0237636, 0.032185, -0.0188489, 0.000650689, 0.0103035, -0.0468485, 0.0387169, 0.0211297, 0.0546025, -0.131823, -0.0093694, 0.0646584, 0.0432479, -0.00394126, -0.120119, 0.135321, -0.0593962, -0.0222644, 0.0988435, 0.00633988, 0.0397465, 0.105725, 0.029232, 0.0687546, -0.0478611, 0.0254317, 0.105443, 0.00574502, 0.0368124, 0.0800893, 0.0734889, -0.0872124, -0.0932197, 0.0377657}}}});

    tensor_4 out(1,10,1,1); out.setZero();

    tensor_4 expected(1,10,1,1);
    expected.setValues({{{{0.326387, 0.227462, -0.263816, -0.0162714, -0.266815, -0.0715114, 0.17066, -0.0171155, 0.16697, 0.129336}}}});

    fc.forward_prop(in, out);

    assert_tensor_equals(expected, out, 1e-2);

}

BOOST_AUTO_TEST_CASE(test_fully_connected_bprop) {

    using layer_type = fully_connected_layer<volume_dims<1, 1, 4>, 2>;
    layer_type fc;
    fc.weight_initializer = fixed_weight_initializer(1.0);
    fc.bias_initializer = fixed_weight_initializer(0.5);
    fc.configure(1, true);

    tensor_4 prev_out(1,1,1,4);
    prev_out.setValues({{{{0, 1, 2, 3}}}});
    tensor_4 curr_out(1,2,1,1);
        curr_out.setZero();
    tensor_4 expected(1,2,1,1);
        expected.setValues({{{{6.5}},{{6.5}}}});
    tensor_4 prev_delta(1,1,1,4);
        prev_delta.setZero();
    tensor_4 curr_delta(1,2,1,1);
        curr_delta.setValues({{{{1}},{{2}}}});
    tensor_4 expected_prev_delta(1,1,1,4);
        expected_prev_delta.setValues({{{{3,3,3,3}}}});

    fc.forward_prop(prev_out, curr_out);
    assert_tensor_equals(expected, curr_out);
    fc.backward_prop(prev_out, prev_delta, curr_out, curr_delta);
    assert_tensor_equals(expected_prev_delta, prev_delta);
}
BOOST_AUTO_TEST_CASE(test_fully_connected_gradient_check) {

    using layer_type = fully_connected_layer<volume_dims<1, 1, 50>, 10>;
    constexpr float_t epsilon = 1e-2f;
    constexpr size_t batch_size = 1;
    layer_type layer;

    layer.weight_initializer = gauss_weight_initializer(-1.0f, 1.0f);
    layer.bias_initializer = gauss_weight_initializer(-1.0f, 1.0f);

    /* setup weights and dimensions */
    layer.configure(batch_size, true);

    auto in = generate_inputs_for(layer, batch_size);

    for(size_t i = 0; i < 100; ++i) {
        auto in_selected  = gradient_random_input(layer);
        auto out_selected = gradient_random_output(layer);

        /* Perform numerical and analytics gradient */
        /* estimate */
        auto n = numerical_gradient (layer, in, in_selected, out_selected);
        /* actual */
        auto a = analytical_gradient(layer, in, in_selected, out_selected);

        /* Validate result */
        BOOST_CHECK_MESSAGE(std::abs(a-n) <= epsilon, "Gradient check |" << std::setprecision(15) << a << " - " << n << "| < " << epsilon);
    }
}
