#!/bin/bash

cat /sys/kernel/debug/page_owner > page_owner_full
grep -v ^PFN page_owner_full > page_owner
tools/vm/page_owner_sort page_owner sorted_page_owner
