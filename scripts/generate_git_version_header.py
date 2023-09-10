#!/usr/bin/env python3

import subprocess
import re


if __name__ == "__main__":
    cp = subprocess.run(["git", "describe", "--tags", "--dirty", "--match", "v*"], capture_output=True, text=True)

    git_ok = False
    major = None
    minor = None
    patch = None
    commit_num = None
    commit_sha = None
    dirty = None

    if cp.returncode == 0:
        git_ok = True

        output = cp.stdout.strip()
        r = re.search('^v([0-9]+)\.([0-9]+)\.([0-9]+)(?:-([0-9]+)-([a-z0-9]+))?(?:-(dirty))?', output)

        major = r.group(1)
        minor = r.group(2)
        patch = r.group(3)
        commit_num = r.group(4)
        commit_sha = r.group(5)
        dirty = r.group(6)

    out = """/*
 * H9 project
 *
 * Created by SQ8KFH on 2023-09-10.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

/* --------------------------------------------------------------------------- *
 * This file is autogenerated by generate_git_version_header.py                *
 * during the build of your project.                                           *
 * --------------------------------------------------------------------------- */

#pragma once

"""

    out = out + ("#define GITVERSION\n" if git_ok else "/* #define GITVERSION */\n")
    out = out + ("#define GITVERSION_MAJOR {}\n".format(major) if major else "/* #define GITVERSION_MAJOR */\n")
    out = out + ("#define GITVERSION_MINOR {}\n".format(minor) if minor else "/* #define GITVERSION_MINOR */\n")
    out = out + ("#define GITVERSION_PATCH {}\n".format(patch) if patch else "/* #define GITVERSION_PATCH */\n")
    out = out + ("#define GITVERSION_COMMIT_SHA \"{}\"\n".format(commit_sha) if commit_sha else "/* #define GITVERSION_COMMIT_SHA */\n")
    out = out + ("#define GITVERSION_DIRTY\n" if dirty else "/* #define GITVERSION_DIRTY */\n")

    print(out)