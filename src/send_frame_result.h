/*
 * H9 project
 *
 * Created by crowx on 2023-09-20.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

struct SendFrameResult {
    std::uint8_t seqnum;
    std::uint16_t source_id;
    unsigned int successfully_sent_to_drivers_count;
    unsigned int failed_to_send_to_drivers_count;
};
