#!/bin/bash
echo 1 > /sys/kernel/debug/tracing/events/jbd2/jbd2_commit_flushing/enable
echo 1 > /sys/kernel/debug/tracing/events/ext4/ext4_sync_file_enter/enable
