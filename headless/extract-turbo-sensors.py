#!/usr/bin/env python3
"""Dump console-calculation sensor rows for one turbo-expander test."""

import argparse
import ctypes
import csv
from pathlib import Path


def configure(lib):
    lib.createTester.argtypes = [ctypes.c_char_p]
    lib.createTester.restype = ctypes.c_bool
    lib.parseArguments.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    lib.parseArguments.restype = ctypes.c_bool
    lib.initSimulation.argtypes = [
        ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.c_char_p),
        ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.POINTER(ctypes.c_double))]
    lib.initSimulation.restype = ctypes.c_bool
    lib.freeBuffer.argtypes = [ctypes.c_char_p]
    lib.freeBuffer.restype = None
    lib.freeSimulationData.argtypes = [ctypes.POINTER(ctypes.c_double)]
    lib.freeSimulationData.restype = None
    lib.runSimulation.argtypes = [ctypes.POINTER(ctypes.c_uint), ctypes.POINTER(ctypes.POINTER(ctypes.c_double))]
    lib.runSimulation.restype = ctypes.c_bool


def sensor_names(lib):
    count = ctypes.c_uint()
    buffer = ctypes.c_char_p()
    size = ctypes.c_uint()
    values = ctypes.POINTER(ctypes.c_double)()
    if not lib.initSimulation(ctypes.byref(count), ctypes.byref(buffer), ctypes.byref(size), ctypes.byref(values)):
        raise RuntimeError("initSimulation failed")
    raw = ctypes.string_at(buffer, size.value)
    names = raw.split(b"\0")[:count.value]
    init_values = [values[index] for index in range(count.value)]
    lib.freeBuffer(buffer)
    lib.freeSimulationData(values)
    return [name.decode("utf-8") for name in names], init_values


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--libdir", required=True)
    parser.add_argument("--config", required=True)
    parser.add_argument("--signals", required=True)
    parser.add_argument("--calc-time", type=float, default=400)
    parser.add_argument("--timestep", type=float, default=1)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    libdir = Path(args.libdir).resolve()
    lib = ctypes.CDLL(str(libdir / "libconsole-calculation.so"))
    configure(lib)
    if not lib.createTester(str(libdir).encode()):
        raise RuntimeError("createTester failed")
    arguments = [
        str(libdir), "--libdir", str(libdir), "--mode", "single",
        args.config, "--signals", args.signals,
        "--calc-time", str(args.calc_time), "--timestep", str(args.timestep),
    ]
    argv = (ctypes.c_char_p * len(arguments))(*[item.encode() for item in arguments])
    if not lib.parseArguments(len(arguments), argv):
        raise RuntimeError("parseArguments failed")

    names, initial_values = sensor_names(lib)
    size = ctypes.c_uint()
    values = ctypes.POINTER(ctypes.c_double)()
    if not lib.runSimulation(ctypes.byref(size), ctypes.byref(values)):
        raise RuntimeError("runSimulation failed")
    flat_values = [values[index] for index in range(size.value)]
    lib.freeSimulationData(values)
    if len(names) == 0 or len(flat_values) % len(names) != 0:
        raise RuntimeError("sensor buffer shape is inconsistent")

    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.writer(stream)
        writer.writerow(["step", *names])
        writer.writerow([0, *initial_values])
        for step in range(len(flat_values) // len(names)):
            begin = step * len(names)
            writer.writerow([step + 1, *flat_values[begin:begin + len(names)]])
    print(output)


if __name__ == "__main__":
    main()
