#include "h9_msg.h"
#include "h9msg.h"

#include <stdio.h>

static const char* const h9_type_name[] = {
        "RESERVED_0",
        "RESERVED_1",
        "RESERVED_2",
        "RESERVED_3",
        "RESERVED_4",
        "RESERVED_5",
        "RESERVED_6",
        "RESERVED_7",
        "RESERVED_8",
        "RESERVED_9",
        "RESERVED_10",
        "RESERVED_11",
        "RESERVED_12",
        "RESERVED_13",
        "RESERVED_14",
        "RESERVED_15",
        "RESERVED_16",
        "RESERVED_17",
        "RESERVED_18",
        "RESERVED_19",
        "RESERVED_20",
        "RESERVED_21",
        "RESERVED_22",
        "RESERVED_23",
        "RESERVED_24",
        "RESERVED_25",
        "RESERVED_26",
        "RESERVED_27",
        "RESERVED_28",
        "RESERVED_29",
        "RESERVED_30",
        "RESERVED_31",
        "RESERVED_32",
        "RESERVED_33",
        "RESERVED_34",
        "RESERVED_35",
        "RESERVED_36",
        "RESERVED_37",
        "RESERVED_38",
        "RESERVED_39",
        "RESERVED_40",
        "RESERVED_41",
        "RESERVED_42",
        "RESERVED_43",
        "RESERVED_44",
        "RESERVED_45",
        "RESERVED_46",
        "RESERVED_47",
        "RESERVED_48",
        "RESERVED_49",
        "RESERVED_50",
        "RESERVED_51",
        "RESERVED_52",
        "RESERVED_53",
        "RESERVED_54",
        "RESERVED_55",
        "RESERVED_56",
        "RESERVED_57",
        "RESERVED_58",
        "RESERVED_59",
        "RESERVED_60",
        "RESERVED_61",
        "RESERVED_62",
        "RESERVED_63",
        "REG_EXTERNALLY_CHANGED",
        "REG_INTERNALLY_CHANGED",
        "REG_VALUE_BROADCAST",
        "REG_VALUE",
        "NODE_HEARTBEAT",
        "NODE_TURNED_ON",
        "RESERVED_70",
        "RESERVED_71",
        "RESERVED_72",
        "RESERVED_73",
        "RESERVED_74",
        "RESERVED_75",
        "RESERVED_76",
        "RESERVED_77",
        "RESERVED_78",
        "RESERVED_79",
        "RESERVED_80",
        "RESERVED_81",
        "RESERVED_82",
        "RESERVED_83",
        "RESERVED_84",
        "RESERVED_85",
        "RESERVED_86",
        "RESERVED_87",
        "RESERVED_88",
        "RESERVED_89",
        "RESERVED_90",
        "RESERVED_91",
        "RESERVED_92",
        "RESERVED_93",
        "RESERVED_94",
        "RESERVED_95",
        "RESERVED_96",
        "RESERVED_97",
        "RESERVED_98",
        "RESERVED_99",
        "RESERVED_100",
        "RESERVED_101",
        "RESERVED_102",
        "RESERVED_103",
        "RESERVED_104",
        "RESERVED_105",
        "RESERVED_106",
        "RESERVED_107",
        "RESERVED_108",
        "RESERVED_109",
        "RESERVED_110",
        "RESERVED_111",
        "RESERVED_112",
        "RESERVED_113",
        "RESERVED_114",
        "RESERVED_115",
        "RESERVED_116",
        "RESERVED_117",
        "RESERVED_118",
        "RESERVED_119",
        "RESERVED_120",
        "RESERVED_121",
        "RESERVED_122",
        "RESERVED_123",
        "RESERVED_124",
        "RESERVED_125",
        "RESERVED_126",
        "RESERVED_127",
        "SET_REG",
        "GET_REG",
        "NODE_INFO",
        "NODE_RESET",
        "RESERVED_132",
        "RESERVED_133",
        "RESERVED_134",
        "RESERVED_135",
        "RESERVED_136",
        "RESERVED_137",
        "RESERVED_138",
        "RESERVED_139",
        "RESERVED_140",
        "RESERVED_141",
        "RESERVED_142",
        "RESERVED_143",
        "RESERVED_144",
        "RESERVED_145",
        "RESERVED_146",
        "RESERVED_147",
        "RESERVED_148",
        "RESERVED_149",
        "RESERVED_150",
        "RESERVED_151",
        "RESERVED_152",
        "RESERVED_153",
        "RESERVED_154",
        "RESERVED_155",
        "RESERVED_156",
        "RESERVED_157",
        "RESERVED_158",
        "RESERVED_159",
        "RESERVED_160",
        "RESERVED_161",
        "RESERVED_162",
        "RESERVED_163",
        "RESERVED_164",
        "RESERVED_165",
        "RESERVED_166",
        "RESERVED_167",
        "RESERVED_168",
        "RESERVED_169",
        "RESERVED_170",
        "RESERVED_171",
        "RESERVED_172",
        "RESERVED_173",
        "RESERVED_174",
        "RESERVED_175",
        "RESERVED_176",
        "RESERVED_177",
        "RESERVED_178",
        "RESERVED_179",
        "RESERVED_180",
        "RESERVED_181",
        "RESERVED_182",
        "RESERVED_183",
        "RESERVED_184",
        "RESERVED_185",
        "RESERVED_186",
        "RESERVED_187",
        "RESERVED_188",
        "RESERVED_189",
        "RESERVED_190",
        "RESERVED_191",
        "DISCOVERY",
        "RESERVED_193",
        "RESERVED_194",
        "RESERVED_195",
        "RESERVED_196",
        "RESERVED_197",
        "RESERVED_198",
        "RESERVED_199",
        "RESERVED_200",
        "RESERVED_201",
        "RESERVED_202",
        "RESERVED_203",
        "RESERVED_204",
        "RESERVED_205",
        "RESERVED_206",
        "RESERVED_207",
        "RESERVED_208",
        "RESERVED_209",
        "RESERVED_210",
        "RESERVED_211",
        "RESERVED_212",
        "RESERVED_213",
        "RESERVED_214",
        "RESERVED_215",
        "RESERVED_216",
        "RESERVED_217",
        "RESERVED_218",
        "RESERVED_219",
        "RESERVED_220",
        "RESERVED_221",
        "RESERVED_222",
        "RESERVED_223",
        "RESERVED_224",
        "RESERVED_225",
        "RESERVED_226",
        "RESERVED_227",
        "RESERVED_228",
        "RESERVED_229",
        "RESERVED_230",
        "RESERVED_231",
        "RESERVED_232",
        "RESERVED_233",
        "RESERVED_234",
        "RESERVED_235",
        "RESERVED_236",
        "RESERVED_237",
        "RESERVED_238",
        "RESERVED_239",
        "RESERVED_240",
        "RESERVED_241",
        "RESERVED_242",
        "RESERVED_243",
        "RESERVED_244",
        "RESERVED_245",
        "RESERVED_246",
        "RESERVED_247",
        "RESERVED_248",
        "RESERVED_249",
        "RESERVED_250",
        "RESERVED_251",
        "RESERVED_252",
        "RESERVED_253",
        "RESERVED_254",
        "RESERVED_255"
};

int h9_msg_parse(const char *msg, size_t msg_size) {
    char *pre = "";
    if (msg[0] == 't') {
        uint32_t id;
        sscanf(msg + 1, "%3x", &id);
        printf("%sframe (%s) SFF (11-bit) id: 0x%X\n", pre, msg, id);
        fprintf(stderr, "slcan 't' frame format not implemented yet\n");
        return 0;
    }
    else if (msg[0] == 'T') {
        uint32_t id;
        sscanf(msg + 1, "%8x", &id);
        printf("%sframe (%s) EFF (29-bit) id: 0x%X\n", pre, msg, id);

        uint32_t msg_priority = (id >> (H9_MSG_TYPE_BIT_LENGTH + H9_MSG_RESERVED_BIT_LENGTH + H9_MSG_DESTINATION_ID_BIT_LENGTH + H9_MSG_SOURCE_ID_BIT_LENGTH)) \
		                        & ((1<<H9_MSG_PRIORITY_BIT_LENGTH) - 1);
        char *priority_t = "UNKNOWN";
        switch (msg_priority) {
            case H9_MSG_PRIORITY_HIGH:
                priority_t = "HIGH";
                break;
            case H9_MSG_PRIORITY_LOW:
                priority_t = "LOW";
                break;
        }
        printf("\th9 priority: %s (%u)\n", priority_t, msg_priority);

        uint32_t msg_type = (id >> (H9_MSG_RESERVED_BIT_LENGTH + H9_MSG_DESTINATION_ID_BIT_LENGTH + H9_MSG_SOURCE_ID_BIT_LENGTH)) \
		                    & ((1<<H9_MSG_TYPE_BIT_LENGTH) - 1);
        printf("\th9 msg type: %s (%u)\n", h9_type_name[msg_type], msg_type);

        uint32_t destination_id = (id >> (H9_MSG_SOURCE_ID_BIT_LENGTH)) \
		                          & ((1<<H9_MSG_DESTINATION_ID_BIT_LENGTH) - 1);
        if (destination_id == H9_MSG_BROADCAST_ID) {
            printf("\th9 destination id: BROADCAST (0x%02X)\n", destination_id);
        }
        else {
            printf("\th9 destination id: %u (0x%02X)\n", destination_id, destination_id);
        }

        uint32_t source_id = (id >> (0)) \
		                     & ((1<<H9_MSG_SOURCE_ID_BIT_LENGTH) - 1);
        printf("\th9 source id: %u (0x%02X)\n", source_id, source_id);

        uint32_t dlc;
        sscanf(msg + 9, "%1u", &dlc);
        printf("\th9 data length: %u\n", dlc);

        printf("\th9 data:");
        for (int i=0; i < dlc; ++i) {
            unsigned int data;
            sscanf(msg + 10 + i*2, "%2x", &data);
            printf(" %02X", data);
        }
        printf("\n");
    }
    else if (msg[0] == 'r') {
        uint32_t id;
        sscanf(msg + 1, "%3x", &id);
        printf("%sframe (%s) RTR/SFF (11-bit) id: 0x%X\n", pre, msg, id);
        fprintf(stderr, "slcan 'r' frame format not implemented yet\n");
        return 0;
    }
    else if(msg[0] == 'R') {
        uint32_t id;
        sscanf(msg + 1, "%8x", &id);
        printf("%sframe (%s) RTR/EFF (29-bit) id: 0x%X\n", pre, msg, id);
        fprintf(stderr, "slcan 'R' frame format not implemented yet\n");
        return 0;
    }
    else {
        printf("%sunknown (%s) (0x%2hhx)\n", pre, msg, msg[0]);
    }
    return 0;
}
