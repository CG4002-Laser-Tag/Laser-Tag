/*
----------------------------------------------------------------------------------
--	(c) Rajesh C Panicker, NUS,
--  Description : Self-checking testbench for AXI Stream Coprocessor (HLS) implementing the sum of 4 numbers
--	License terms :
--	You are free to use this code as long as you
--		(i) DO NOT post a modified version of this on any public repository;
--		(ii) use it only for educational purposes;
--		(iii) accept the responsibility to ensure that your implementation does not violate any intellectual property of any entity.
--		(iv) accept that the program is provided "as is" without warranty of any kind or assurance regarding its suitability for any particular purpose;
--		(v) send an email to rajesh.panicker@ieee.org briefly mentioning its use (except when used for the course EE4218 at the National University of Singapore);
--		(vi) retain this notice in this file or any files derived from this.
----------------------------------------------------------------------------------
*/

#include <stdio.h>
#include "hls_stream.h"
#include "ap_int.h"

/***************** AXIS with TLAST structure declaration *********************/

struct AXIS_wLAST{
	double data;
	bool last;
};

/***************** Coprocessor function declaration *********************/

void mlp_solution_hls_left(hls::stream<AXIS_wLAST>& S_AXIS, hls::stream<AXIS_wLAST>& M_AXIS);


/***************** Macros *********************/
#define NUMBER_OF_INPUT_WORDS 16  // length of an input vector
#define NUMBER_OF_OUTPUT_WORDS 5  // length of an input vector
#define NUMBER_OF_TEST_VECTORS 5  // number of such test vectors (cases)

#define HIDDEN_LAYER_1_SIZE 32

/************************** Variable Definitions *****************************/
//int test_input_memory [NUMBER_OF_TEST_VECTORS*NUMBER_OF_INPUT_WORDS] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05,0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05, 0x01, 0x02, 0x03, 0x04, 0x05}; // 4 inputs * 2
/**
double test_input_memory [NUMBER_OF_TEST_VECTORS*NUMBER_OF_INPUT_WORDS] = {0.3050025203548387,0.4475195293879845,0.5355742933800642,-1.1953125,0.96484375,-0.9778855847741936,0.7852111311764203,1.2461652448273612,-2.0,1.1484375,-0.059475806451612906,0.4977829006761008,0.4932869675930172,-1.047851563,0.912109375,-39.875504032258064,131.01913473818806,134.91600021123415,-220.734375,232.3203125,49.366431451612904,148.65692425640162,154.3472013041854,-228.4375,252.65625,91.27923387096774,137.9288067558928,163.5315424453612,-219.5390625,248.1328125,
		0.3169417843870968,0.4084586209243899,0.5117701186813375,-0.467773438,1.34375,-0.8568233367741936,0.8948316197908217,1.2284299181369258,-2.0,0.543945313,-0.09359249003225807,0.33352758132733495,0.3411917257385264,-1.076171875,0.71484375,-67.34122983870968,127.793012907188,142.61517165353547,-233.625,236.671875,66.59047379032258,172.70383566973325,182.47947059427344,-239.5859375,252.65625,84.46925403225806,141.50623043705326,162.8285362517868,-191.6640625,211.9453125,
		0.15439138106451614,0.8877503649687362,0.8868566995205867,-2.0,1.74609375,-0.9129599295161291,1.1415777467428188,1.4472929499889284,-2.0,1.33203125,0.04277973796774194,0.9347346037042363,0.9205292423429268,-1.372070313,1.999023438,18.95967741935484,100.93790777895842,101.09040782915818,-154.0234375,230.421875,-58.64969758064516,147.71209947546095,156.69976714876466,-252.4453125,214.2734375,-155.87399193548387,151.9035059804686,215.93293433854979,-249.90625,194.2578125,
		-0.32242313519354837,0.6787079913568423,0.741445692164287,-2.0,1.943359375,-0.5710055443548386,0.9702599748730227,1.11224271903476,-2.0,0.645507813,0.3726373488387097,1.1137707879895404,1.1572935258137227,-1.259765625,1.999023438,-79.29233870967742,175.1815326912453,189.69947422862944,-249.859375,250.3984375,-26.375252016129032,104.37999057281955,106.0159367882506,-221.578125,251.8203125,-27.748991935483872,147.03865405637507,147.28523750103363,-249.0,251.2578125,
		0.09926285296774194,0.7731717172087295,0.7670488410681348,-2.0,1.791015625,-0.8021988408064517,1.1236014777665517,1.3657518408117717,-2.0,1.111328125,-0.12427545370967742,0.5828807513400464,0.5867151627636273,-0.928710938,1.864257813,22.617691532258064,141.75615769183565,141.273311680024,-206.0546875,252.5546875,-34.64415322580645,151.78047538093705,153.27880133932467,-252.6796875,232.046875,-95.41154233870968,148.7758034860163,174.71002942592963,-235.359375,239.140625,
		-0.30276587725806453,0.3387097831356468,0.4502118492422212,-0.833984375,0.701171875,-0.8042149698709679,0.7574283161898684,1.0963361692427807,-2.0,0.294921875,-0.1682837702903226,0.43214940360948406,0.45721793557085433,-1.924804688,0.51953125,17.819808467741936,130.55094620957502,129.6584097113416,-254.6484375,245.6171875,-7.902721774193548,166.9619185266905,164.43691902564544,-249.6640625,250.59375,0.3528225806451613,157.41581001934372,154.85643469067847,-253.171875,247.0859375,
		-0.2807144658387097,0.47477504285852173,0.5449225807960367,-1.6484375,0.92578125,-1.0569556453548388,0.7643346753536477,1.297118856967369,-2.0,0.404296875,-0.1651020665483871,0.5650098930370298,0.5798249135237199,-2.0,1.138671875,39.040826612903224,137.5630601832346,140.84510323120458,-194.5078125,245.6171875,-9.60710685483871,158.17987531312352,155.90395928966387,-249.6640625,250.59375,24.602318548387096,173.6227617000435,172.56222793691896,-253.171875,247.0859375,
		-0.17578125006451614,0.856245708435185,0.8604682123894154,-2.0,1.999023438,-1.071005544483871,0.986115552930799,1.4450288051346092,-2.0,0.958984375,-0.5073399698709677,0.5761563642882648,0.7606850452310916,-1.362304688,0.772460938,-6.212953629032258,148.79492124644554,146.5071265731961,-211.9453125,196.015625,-40.379788306451616,135.0771173340183,138.88045163445938,-251.328125,231.2890625,-102.69934475806451,149.94298278692494,179.73535746695632,-248.6640625,173.3984375,
		-0.5366368448387098,0.5127395115719681,0.7364782457905306,-1.271484375,1.227539063,-0.8979964719677418,0.729027151866202,1.149231794166339,-2.0,0.36328125,0.3761340726129032,1.0904113123837145,1.1367140952312964,-1.494140625,1.876953125,-110.92338709677419,174.9479783101961,204.75223307304782,-235.6484375,243.3046875,-33.23991935483871,75.32407447455965,81.2132090924019,-173.0390625,242.953125,10.662802419354838,154.68553593419534,152.54327855863508,-249.25,251.0078125,
		-0.41885080667741936,0.4291139310551179,0.5946720137117432,-1.192382813,0.37890625,-0.9826738912903227,0.8060580227211184,1.2627028531395041,-2.0,0.26953125,-0.2615297380322581,0.5680058020980563,0.6169448570706472,-1.752929688,0.859375,32.46648185483871,129.92806062071406,131.8742404185122,-199.7578125,245.6171875,-16.007056451612904,179.61911985076108,177.4218454039158,-249.6640625,250.59375,-25.854334677419356,174.75120515113957,173.84284303629903,-253.171875,247.0859375};
*/
double test_input_memory [NUMBER_OF_TEST_VECTORS*NUMBER_OF_INPUT_WORDS] = {-1.038234786343186,-1.765201670938282,0.13472099777228266,0.42304342116256893,0.43639072737641177,-0.31307615405754713,3.151043151885161,1.0472150996579457,-0.006990411077838341,-2.3506714858691042,-1.866180628202496,1.1489784589448715,0.2934779419834666,-0.3342495835139358,0.28183525547130434,-0.48552302052448687,
		1.1428660721993595,-0.9380204745842877,1.415930768316793,-0.19707456156791686,-0.8665389946577167,0.9053320373136988,1.3098322436983256,1.3480092567668953,0.08655659990020718,1.0498837827660432,-0.35697012599003347,0.45927574834087903,-0.34178398299876095,-0.3991559868036585,-0.08840341188172698,-0.13700024302730301,
		-2.149635256080148,3.64449832268638,3.0020568822737554,0.3071743422935293,-0.2625017963837881,-0.692064323011517,2.506604428762938,-3.1176380193336937,-0.6321265124757905,-0.28224056424400135,1.9338936105353646,1.2136616120571895,-0.28647898262649973,-0.323405491784717,0.12450565247385424,0.40618225229687055,
		1.992804412509422,-4.705064399295772,1.6283725753060125,-0.30554838944182744,0.43745570336735107,-0.1841803287947272,-1.6317321820395514,0.34557916629255486,-1.327796706141693,0.42139211048293707,0.23892692012964606,-0.008189788101154586,-0.16128588780803246,0.6726437202938959,0.2742706741639343,0.14199346785297007,
		-0.23267099951443357,1.8561781922647402,-2.9826762400354796,0.4930470161916122,0.5683499168039624,-2.3771508842203986,0.16998053277407993,1.2487897847341072,-0.21995574788336203,-0.32235480331057853,-0.9636378502722854,0.25956493431172023,0.2214638174213324,0.03338071968237924,-0.40669546715108407,-0.1870808800908301};


double test_actual_prediction [NUMBER_OF_OUTPUT_WORDS*NUMBER_OF_TEST_VECTORS] = {2,3,1,3,2};

double test_result_expected_memory [NUMBER_OF_TEST_VECTORS*NUMBER_OF_OUTPUT_WORDS];// 4 outputs *2
double result_memory [NUMBER_OF_TEST_VECTORS*NUMBER_OF_OUTPUT_WORDS]; // same size as test_result_expected_memory

int c_result_prediction[NUMBER_OF_TEST_VECTORS];
int rtl_result_prediction[NUMBER_OF_TEST_VECTORS];

// ================ weights ===============
double input_weights[NUMBER_OF_INPUT_WORDS][HIDDEN_LAYER_1_SIZE] = {{0.44862407,0.45225134,0.1762152,0.2792677,0.0682438,-0.48238114,0.14130154,-0.074665554,0.28133062,-0.27899334,0.25928468,-0.61511314,0.0794439,-0.3584254,-0.53937787,0.3119785,-0.020534756,0.22621076,0.41437832,-0.087727696,-0.3531873,-0.064924374,-0.0371849,-0.38588685,0.044347987,-0.6330173,-0.0011001904,-0.020461416,0.22666998,0.35026544,0.39843714,-0.18158968},
		{0.1611541,-0.24681208,0.15060794,-0.38694456,0.327627,-0.5165929,0.010336923,-0.4901953,-0.3576806,0.41572154,0.23272291,-0.11210011,0.7719164,0.29807717,0.5880553,0.26314878,-0.3388027,0.5732292,-0.3175772,0.3508989,0.017166534,-0.09852137,0.17491382,0.37056467,0.12652254,0.013051268,0.088475645,0.33504674,-0.42472342,-0.4694984,0.24310085,0.7205194},
		{-0.14744781,0.03303504,-0.41167465,-0.009457934,0.5013788,0.19457974,-0.53255916,-0.11959485,0.42690462,0.31753427,0.062229007,0.4016663,-0.23917039,0.44011185,0.06326522,-0.20585771,0.1333792,0.43507037,0.31708512,-0.5424209,0.64035517,-0.40637726,0.1738139,0.42609727,-0.7891431,0.2217225,-0.7170458,-0.53380847,0.31932932,0.021626458,-0.27955022,-0.33448163},
		{0.029573685,-0.18369234,0.553056,-0.39674902,-0.4678998,0.2613258,-0.2304312,0.10679686,-0.49163058,-0.061225154,-0.31100386,-0.11555496,0.18702693,-0.16282818,0.013994778,-0.059286818,0.37472057,-0.36060348,-0.58841467,0.4330827,-0.09189659,0.4530957,-0.22721507,-0.090485595,0.24744156,0.20733824,0.007915538,0.47912958,-0.6437411,-0.4795327,-0.099007405,0.1651328},
		{-0.106074736,0.18763071,-0.30438596,-0.23485948,0.0875431,0.11290652,-0.010078681,0.09581408,-0.059100553,0.29120168,0.016851798,0.060726587,-0.1305054,-0.06911616,-0.08690631,-0.32109696,0.09575311,-0.06957441,-0.10629194,-0.27694026,0.048800137,-0.16852042,0.24943492,0.037956376,0.17066085,-0.15425476,-0.11433208,-0.21467839,-0.09538321,0.32846808,-0.37048885,-0.11319243},
		{0.012191007,0.2292665,0.22527361,0.7060442,0.23015018,-0.68208385,-0.36676276,-0.6110861,0.62692934,0.13369575,-0.44618312,-0.25322813,-0.11180859,-0.004988474,0.24052738,-0.4631205,-0.59484303,0.17490982,0.31630814,0.0057240073,-0.1192524,-0.4531151,0.45626122,0.4317206,-0.06337249,-0.57425004,-0.42638198,0.046859793,0.5734962,0.45634848,-0.33034205,-0.17587057},
		{-0.5176102,-0.48055077,0.025276687,-0.0012032362,0.21336788,0.33118343,0.13117795,-0.026806207,-0.29407626,0.20126751,-0.0358727,0.21286844,0.22491671,0.03076073,0.08437297,0.07628481,0.24380666,0.29489598,-0.16328269,0.29086092,-0.20289753,-0.40076515,-0.22927898,-0.08435484,-0.20053275,-0.07913426,-0.38712484,0.09170975,-0.0929295,-0.09823241,0.1169015,0.18586521},
		{0.4457616,0.5219247,-0.48370788,0.2335369,-0.013852411,-0.19608387,-0.38548717,-0.043046366,0.25592023,-0.16604096,0.052442614,-0.07256186,-0.27139273,0.23792349,-0.014754649,-0.23960967,-0.18249601,-0.44407132,0.1660197,-0.07185881,-0.16919068,-0.19255869,0.3142061,-0.0967977,-0.14058319,-0.24458498,0.06264932,-0.029128788,0.37840185,0.47002867,-0.2951978,-0.19321579},
		{0.4442606,0.053753898,-0.050644014,-0.1318523,0.3386272,-0.04128086,-0.041869827,0.25197053,-0.11429896,0.02149327,0.034978002,0.38313967,-0.3111218,0.28410152,0.064994074,-0.21802083,0.3874341,-0.118275285,-0.034527406,-0.33203065,-0.07532363,-0.18501365,0.055767663,0.08900559,0.2452055,-0.23896314,0.27006638,-0.2017661,-0.090691,0.31196907,0.16929112,0.0045021214},
		{-0.014481599,-0.10017341,-0.045926478,0.1507445,-0.012524216,-0.13289802,-0.3029377,-0.30223274,0.012690222,-0.044287425,-0.2653105,-0.051642545,-0.28268537,0.047327295,0.17639244,-0.0886041,0.059776437,-0.079722255,0.1926446,-0.076135784,0.10706656,-0.10241177,-0.14852504,0.06252844,0.13617769,-0.14178921,0.09192267,-0.057671305,0.33035114,0.12207688,0.31897435,-0.052370027},
		{0.33308902,-0.12138532,0.08690389,0.23118429,-0.32856148,-0.1948419,-0.2961348,-0.022260213,0.12239171,0.18860726,-0.26986492,0.12773032,0.18911101,0.23808901,-0.028450644,-0.06958385,-0.13016398,-0.13008085,-0.05858468,0.0869965,0.16784087,0.27925295,-0.2630144,0.037721537,0.18236914,-0.393946,0.08668787,-0.07484668,0.082490765,-0.08782793,-0.0011475999,-0.20526017},
		{-0.25372374,-0.29973787,0.10647255,0.121698484,-0.26373968,0.09624571,-0.2151535,-0.26322138,0.05416493,-0.16618276,-0.0010204004,0.026454443,0.16520229,-0.044884,0.11826369,-0.091165066,-0.06097809,-0.12096032,-0.076056175,0.18628296,0.13145298,-0.24313253,0.09416286,-0.057318393,-0.19064292,-0.01101809,-0.30127132,0.35593033,0.02875497,-0.020864889,-0.0934015,0.025750393},
		{-0.00061727106,-0.08020582,0.2858664,0.1631885,-0.25943083,0.20187978,-0.37664878,-0.35932904,0.2645024,0.04958911,0.31527367,0.028375378,0.028078223,-0.39794576,-0.247555,0.22000486,-0.2687777,-0.33598012,0.13231632,0.25674263,-0.3666925,-0.14648543,0.17806344,0.01737655,-0.16917959,0.26171362,-0.09663078,0.24573268,0.13671236,0.06727493,0.18971868,0.014628429},
		{0.08675203,-0.2674465,-0.1747775,-0.17676432,0.24426451,0.0001315271,-0.33409813,-0.28876507,-0.08191272,0.4199775,-0.024804022,0.072901964,-0.064537704,0.2578247,-0.050044656,-0.018813638,-0.28971675,0.00045597882,0.10582961,0.20537303,-0.14329287,-0.1329262,0.3142107,-0.21002916,-0.08636404,0.061437286,-0.08720104,-0.13422042,0.18600853,-0.11249476,-0.07159254,-0.06437825},
		{-0.05853192,0.14974867,0.058914118,0.02373894,0.061751746,-0.24777173,0.27395955,-0.013149958,0.007231408,0.124676846,-0.045379452,-0.07209088,-0.07464985,-0.21972257,-0.23103058,-0.04840537,0.23085938,-0.18142974,-0.47538003,0.10307232,-0.04592747,0.123623244,0.2208543,0.26611868,0.1081991,0.07802131,0.08858456,-0.08670145,-0.04817561,-0.21563697,-0.0038287672,0.1406046},
		{-0.20096424,-0.18194401,-0.11762917,0.014702063,0.3289757,-0.15825133,0.023286583,-0.019125953,-0.2687362,-0.13422792,-0.20879178,-0.08416033,-0.43195775,-0.048980817,-0.26370576,-0.35843205,-0.10941079,0.32260755,-0.19314744,-0.028124351,-0.16913523,0.042779475,-0.189529,0.3328904,-0.13554439,-0.08575153,-0.019314878,0.09151835,-0.24922626,-0.003154305,-0.067400195,-0.4192648}};

	double hidden_1_weights[HIDDEN_LAYER_1_SIZE][NUMBER_OF_OUTPUT_WORDS] = {{0.14729846,-0.5493655,0.3968932,-0.08348137,-0.058858316},
			{0.038174503,-0.49678898,0.24154617,-0.60704625,0.18656473},
			{0.25817907,-0.6411304,0.09787585,0.4733634,-0.3813373},
			{0.033953015,-0.8000131,-0.34927756,-0.4340072,0.57310206},
			{0.23345873,0.34654298,-0.31960958,-0.7902086,0.12354009},
			{-0.9331062,-0.5935022,0.5934921,-0.77922946,-0.17906289},
			{0.032352697,-0.17673673,0.4084603,0.17600589,-0.72128123},
			{-0.8459154,-0.4790159,0.45413634,-0.36251354,-0.18213807},
			{-0.12647168,-0.3681519,-0.23976141,-0.64383286,0.554813},
			{-0.4050842,0.3929032,-0.5113163,-0.25369078,-0.042656954},
			{0.24590833,-0.5995853,-0.26195154,0.18547909,0.03712261},
			{-1.1882322,0.20129272,0.29018456,-0.7590323,-0.0071086865},
			{0.36645445,0.054553475,-0.6180989,0.5379525,-0.7424837},
			{-0.71813977,0.29630393,0.03337695,-0.46565536,0.05413709},
			{-0.81920314,0.3513643,-0.37582612,-0.0531027,-0.33630496},
			{0.5451464,-0.3183767,-0.13697074,0.3345962,-0.39612105},
			{-0.11363207,-0.5884998,0.51479226,-0.19423614,-0.52783495},
			{0.6027503,0.3153103,-0.539549,-0.4274411,-0.34162587},
			{0.13672617,-0.6866286,-0.16719188,-0.59491456,0.38896677},
			{-0.84982824,-0.21287158,-0.23629181,0.608717,-0.35067454},
			{-0.94025075,0.10197076,0.008291913,-0.4945412,-0.06161654},
			{-0.93132657,-0.66459495,0.39328733,0.23502272,-0.22610977},
			{-0.5336688,0.056345075,-0.29736218,-0.26567426,0.22819628},
			{-0.738867,0.30546558,-0.22031023,-0.07632538,0.02952168},
			{-0.84261763,-0.011698344,0.14415517,0.27287245,-0.33431756},
			{-1.0144256,0.23745333,0.46745473,-0.10585004,-0.47806364},
			{-0.85377663,-0.2058691,0.28464353,0.15044537,-0.7014917},
			{-0.8848429,-0.31301537,-0.1704494,0.39890784,-0.0859293},
			{-0.21433848,-0.4272517,-0.5005095,-0.5920113,0.63344896},
			{-0.29296255,-0.8174164,-0.026643574,-0.6593131,0.26504526},
			{0.48362607,-0.46897689,0.05160185,0.036396846,-0.64947754},
			{-0.3875898,0.16319141,-0.654627,0.36135027,-0.36397356}};

	// =============================== biases ==================================
	double hidden_1_bias[HIDDEN_LAYER_1_SIZE] = {0.2662889,0.09905708,0.43109977,-0.051076412,0.25265822,0.087114386,0.40691254,0.24488026,0.21420632,0.10204671,-0.02637293,0.6048321,0.57182807,-0.09028275,0.17908145,0.09874614,0.40399683,0.135904,0.42075503,0.20616803,0.22865474,0.6879945,0.13423182,0.16017406,0.25091824,0.3638897,0.24949114,0.36295456,-0.0069031343,0.30983782,0.19810303,0.29781118};

	double output_bias[NUMBER_OF_OUTPUT_WORDS] = {-0.56657994,-0.2381266,0.28228527,0.07756669,-0.1686023};


/*****************************************************************************
* Main function
******************************************************************************/
int main()
{
	int word_cnt, test_case_cnt = 0;
	int success;
	AXIS_wLAST read_output, write_input;
	hls::stream<AXIS_wLAST> S_AXIS;
	hls::stream<AXIS_wLAST> M_AXIS;

	/************** Run a software version of the hardware function to validate results ************/
	// instead of hard-coding the results in test_result_expected_memory
	double input_arr[NUMBER_OF_TEST_VECTORS*NUMBER_OF_INPUT_WORDS];

	double hidden1[HIDDEN_LAYER_1_SIZE];
	double output[NUMBER_OF_OUTPUT_WORDS];
	double output_softmax[NUMBER_OF_OUTPUT_WORDS];

	for (test_case_cnt=0 ; test_case_cnt < NUMBER_OF_TEST_VECTORS ; test_case_cnt++){
		for (word_cnt=0 ; word_cnt < NUMBER_OF_INPUT_WORDS ; word_cnt++){
			input_arr[word_cnt] = test_input_memory[word_cnt+test_case_cnt*NUMBER_OF_INPUT_WORDS];
			//sum +=test_input_memory[word_cnt+test_case_cnt*NUMBER_OF_INPUT_WORDS];
		}
		for(int i = 0; i < HIDDEN_LAYER_1_SIZE; i++) {
			double connection = 0;
			for(int j = 0; j < NUMBER_OF_INPUT_WORDS; j++) {
				connection += input_arr[j] * input_weights[j][i];
			}
			connection += hidden_1_bias[i];
			hidden1[i] = connection > 0 ? connection : 0.01 * connection;
		}

		for(int i = 0; i < NUMBER_OF_OUTPUT_WORDS; i++) {
			double connection = 0;
			for(int j = 0; j < HIDDEN_LAYER_1_SIZE; j++) {
				connection += hidden1[j] * hidden_1_weights[j][i];
			}
			connection += output_bias[i];
			output[i] = connection;
		}

		double m = -INFINITY;
		for (long int i = 0; i < NUMBER_OF_OUTPUT_WORDS; i++){
			if (output[i] > m) {
				m = output[i];
			}
		}

		double sum = 0.0;
			for (size_t i = 0; i < NUMBER_OF_OUTPUT_WORDS; i++){
			sum += expf(output[i] - m);
		}

		double offset = m + logf(sum);
		for (size_t i = 0; i < NUMBER_OF_OUTPUT_WORDS; i++){
			output_softmax[i] = expf(output[i] - offset);
		}

		float max = 0;
		for (word_cnt=0; word_cnt < NUMBER_OF_OUTPUT_WORDS; word_cnt++) {
			test_result_expected_memory[word_cnt+test_case_cnt*NUMBER_OF_OUTPUT_WORDS] = output_softmax[word_cnt];
			if (output_softmax[word_cnt] > max) {
				max = output_softmax[word_cnt];
				c_result_prediction[test_case_cnt] = word_cnt;
			}
		}
	}

	for (test_case_cnt=0 ; test_case_cnt < NUMBER_OF_TEST_VECTORS ; test_case_cnt++){


		/******************** Input to Coprocessor : Transmit the Data Stream ***********************/

		printf(" Transmitting Data for test case %d ... \r\n", test_case_cnt);

		for (word_cnt=0 ; word_cnt < NUMBER_OF_INPUT_WORDS ; word_cnt++){

			write_input.data = test_input_memory[word_cnt+test_case_cnt*NUMBER_OF_INPUT_WORDS];
			write_input.last = 0;
			if(word_cnt==NUMBER_OF_INPUT_WORDS-1)
			{
				write_input.last = 1;
				// S_AXIS_TLAST is asserted for the last word.
				// Actually, doesn't matter since we are not making using of S_AXIS_TLAST.
			}
			S_AXIS.write(write_input); // insert one word into the stream
		}

		/* Transmission Complete */

		/********************* Call the hardware function (invoke the co-processor / ip) ***************/

		mlp_solution_hls_left(S_AXIS, M_AXIS);


		/******************** Output from Coprocessor : Receive the Data Stream ***********************/

		printf(" Receiving data for test case %d ... \r\n", test_case_cnt);
		float max = 0;
		for (int j=0 ; j < NUMBER_OF_OUTPUT_WORDS ; j++){

			read_output = M_AXIS.read(); // extract one word from the stream
			result_memory[j] = read_output.data;

			if (result_memory[j] > max) {
				max = result_memory[j];
				rtl_result_prediction[test_case_cnt] = j;

			}
		}

		/* Reception Complete */
	}

	/************************** Checking correctness of results *****************************/

	success = 1;

	/* Compare the data send with the data received */
	printf(" Comparing data ...\r\n");
	printf("c results:");
	for(int i = 0; i < NUMBER_OF_TEST_VECTORS; i++){
		std::cout << c_result_prediction[i];
	}
	std::cout << "\n";

	printf("rtl results: ");
	for(int i = 0; i < NUMBER_OF_TEST_VECTORS; i++){
		std::cout << rtl_result_prediction[i];
	}
	std::cout << "\n";

	int c_correct_prediction = 0;
	int rtl_correct_prediction = 0;
	for(word_cnt=0; word_cnt < NUMBER_OF_TEST_VECTORS; word_cnt++){

		if(rtl_result_prediction[word_cnt] == test_actual_prediction[word_cnt]) {
			rtl_correct_prediction++;
		}
		if(c_result_prediction[word_cnt] == test_actual_prediction[word_cnt]) {
			c_correct_prediction++;
		}
	}

	if(c_correct_prediction == rtl_correct_prediction) {
		success = 1;
	} else {
		success = 0;
	}

	if (success != 1){
		printf("Test Failed\r\n");
		return 1;
	}

	printf("Test Success\r\n");

	return 0;
}
