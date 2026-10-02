#!/usr/bin/env python3

import json
import sys
from pathlib import Path
from collections import Counter


# ============================================================
# Configuration
# ============================================================

VISUAL_KEYS = {
    "Colour",
    "Color",
    "NameLocation",
    "Size",
    "Rotation",
    "Flipped",
    "FlipX",
    "FlipY",
    "Layer",
    "Selected",
    "IsSelected",
    "Hovered",
    "Visible",
    "ZIndex",
    "TextSize",
    "TextColour",
    "TextColor",
    "Font",
    "FontSize",
    "Opacity",
    "Alpha",
}

VISUAL_SUBSTRINGS = (
    "colour",
    "color",
    "texture",
    "sprite",
    "thumbnail",
    "cache",
    "cached",
)


# ============================================================
# Generic helpers
# ============================================================

def norm(key):
    return (
        str(key)
        .lower()
        .replace("_", "")
        .replace("-", "")
        .replace(" ", "")
    )


def get(obj, *names):
    if not isinstance(obj, dict):
        return None

    keys = {
        norm(key): key
        for key in obj
    }

    for name in names:
        key = keys.get(norm(name))

        if key is not None:
            return obj[key]

    return None


def is_visual(key):
    if key in VISUAL_KEYS:
        return True

    key = str(key).lower()

    return any(
        x in key
        for x in VISUAL_SUBSTRINGS
    )


# ============================================================
# Compact recursive cleaner
# ============================================================

def clean(value):
    if isinstance(value, dict):

        result = {}

        for key, child in value.items():

            if is_visual(key):
                continue

            result[key] = clean(child)

        return result

    if isinstance(value, list):

        return [
            clean(x)
            for x in value
        ]

    return value


# ============================================================
# Component extraction
# ============================================================

def component_name(obj):
    return (
        get(
            obj,
            "Name",
            "name",
        )
        or get(
            obj,
            "ChipName",
            "chipName",
        )
        or get(
            obj,
            "ChipType",
            "chipType",
            "Type",
            "type",
        )
        or "UNKNOWN"
    )


def component_id(obj):
    return get(
        obj,
        "ID",
        "Id",
        "id",
    )


def component_type(obj):
    return get(
        obj,
        "ChipType",
        "chipType",
        "Type",
        "type",
    )


def component_label(obj):
    return get(
        obj,
        "Label",
        "label",
    )


def extract_components(data):
    """
    Find likely DLS components recursively.
    """

    components = []

    def walk(value, path="$"):

        if isinstance(value, dict):

            cid = component_id(value)
            name = component_name(value)
            ctype = component_type(value)
            label = component_label(value)

            # A component normally has an ID plus an identity.
            if cid is not None and (
                get(value, "Name", "name") is not None
                or get(value, "ChipType", "chipType") is not None
                or get(value, "Type", "type") is not None
            ):

                components.append({
                    "id": cid,
                    "name": name,
                    "type": ctype,
                    "label": label,
                    "path": path,
                })

            for key, child in value.items():

                if is_visual(key):
                    continue

                walk(
                    child,
                    f"{path}.{key}",
                )

        elif isinstance(value, list):

            for index, child in enumerate(value):

                walk(
                    child,
                    f"{path}[{index}]",
                )

    walk(data)

    return components


# ============================================================
# Wire extraction
# ============================================================

def find_wire_lists(data):
    """
    Locate arrays whose keys look like wire collections.
    """

    result = []

    def walk(value, path="$"):

        if isinstance(value, dict):

            for key, child in value.items():

                if (
                    "wire" in norm(key)
                    or "connection" in norm(key)
                ):

                    if isinstance(child, list):

                        result.append(
                            (path + "." + key, child)
                        )

                walk(
                    child,
                    path + "." + key,
                )

        elif isinstance(value, list):

            for i, child in enumerate(value):

                walk(
                    child,
                    f"{path}[{i}]",
                )

    walk(data)

    return result


# ============================================================
# Wire endpoint extraction
# ============================================================

def extract_numbers(value):
    """
    Recursively extract numeric values.

    Used only as a fallback when the DLS wire structure does
    not expose named endpoint fields.
    """

    result = []

    if isinstance(value, (int, float)):

        result.append(value)

    elif isinstance(value, dict):

        for child in value.values():

            result.extend(
                extract_numbers(child)
            )

    elif isinstance(value, list):

        for child in value:

            result.extend(
                extract_numbers(child)
            )

    return result


def endpoint_candidates(value):
    """
    Find likely endpoint objects.

    DLS versions can represent wire points differently,
    so this intentionally preserves several possible forms.
    """

    candidates = []

    def walk(obj):

        if isinstance(obj, dict):

            keys = {
                norm(k): k
                for k in obj
            }

            endpoint_keys = (
                "id",
                "component",
                "componentid",
                "chip",
                "chipid",
                "pin",
                "pinid",
                "port",
                "portid",
            )

            if any(
                key in keys
                for key in endpoint_keys
            ):

                candidates.append(obj)

            for child in obj.values():
                walk(child)

        elif isinstance(obj, list):

            for child in obj:
                walk(child)

    walk(value)

    return candidates


# ============================================================
# Compact wire representation
# ============================================================

def compact_wire(wire):
    """
    Keep only the first and last logical endpoints.

    If the DLS representation exposes explicit endpoint
    objects, preserve them.

    Otherwise preserve only the first and last coordinate-like
    values instead of the entire wire path.
    """

    # --------------------------------------------------------
    # Common endpoint fields
    # --------------------------------------------------------

    start = get(
        wire,
        "Start",
        "start",
        "From",
        "from",
        "Source",
        "source",
    )

    end = get(
        wire,
        "End",
        "end",
        "To",
        "to",
        "Target",
        "target",
    )

    if start is not None or end is not None:

        return {
            "from": clean(start),
            "to": clean(end),
        }

    # --------------------------------------------------------
    # Explicit endpoint candidates
    # --------------------------------------------------------

    endpoints = endpoint_candidates(wire)

    if len(endpoints) >= 2:

        return {
            "from": clean(endpoints[0]),
            "to": clean(endpoints[-1]),
        }

    # --------------------------------------------------------
    # Coordinate/path fallback
    # --------------------------------------------------------

    # Look for common point/path fields.
    points = None

    for key in (
        "Points",
        "points",
        "Path",
        "path",
        "Positions",
        "positions",
        "Coordinates",
        "coordinates",
    ):

        points = get(wire, key)

        if isinstance(points, list):
            break

    if isinstance(points, list) and points:

        return {
            "from": clean(points[0]),
            "to": clean(points[-1]),
        }

    # --------------------------------------------------------
    # Last-resort representation
    # --------------------------------------------------------

    numbers = extract_numbers(wire)

    if len(numbers) >= 4:

        return {
            "from": numbers[:2],
            "to": numbers[-2:],
        }

    # Never silently throw a wire away.
    return {
        "raw": clean(wire),
    }


# ============================================================
# Compact circuit
# ============================================================

def build_compact(data):

    components = extract_components(
        data
    )

    wire_lists = find_wire_lists(
        data
    )

    wires = []

    for section_path, wire_list in wire_lists:

        for index, wire in enumerate(
            wire_list
        ):

            wires.append({
                "section": section_path,
                "index": index,
                "connection": compact_wire(
                    wire
                ),
            })

    # --------------------------------------------------------
    # Root-level information
    # --------------------------------------------------------

    result = {
        "DLSVersion": get(
            data,
            "DLSVersion",
        ),
        "Name": get(
            data,
            "Name",
        ),
        "ChipType": get(
            data,
            "ChipType",
        ),
        "Inputs": clean(
            get(
                data,
                "Inputs",
                "Input",
            )
        ),
        "Outputs": clean(
            get(
                data,
                "Outputs",
                "Output",
            )
        ),
        "Components": components,
        "Connections": wires,
    }

    return result


# ============================================================
# Human-readable compact representation
# ============================================================

def component_stats(components):

    counter = Counter()

    for component in components:

        name = (
            component["name"]
            or component["type"]
            or "UNKNOWN"
        )

        counter[name] += 1

    return counter


def make_prompt(compact):

    lines = []

    lines.append(
        "DLS COMPACT CIRCUIT DESCRIPTION"
    )

    lines.append("=" * 60)

    lines.append("")

    if compact["Name"] is not None:
        lines.append(
            f"CIRCUIT: {compact['Name']}"
        )

    if compact["DLSVersion"] is not None:
        lines.append(
            f"DLS VERSION: {compact['DLSVersion']}"
        )

    lines.append("")

    # --------------------------------------------------------
    # Inputs / outputs
    # --------------------------------------------------------

    lines.append("INPUTS")

    lines.append("-" * 60)

    if compact["Inputs"] is not None:
        lines.append(
            json.dumps(
                compact["Inputs"],
                ensure_ascii=False,
            )
        )
    else:
        lines.append("NONE")

    lines.append("")

    lines.append("OUTPUTS")

    lines.append("-" * 60)

    if compact["Outputs"] is not None:
        lines.append(
            json.dumps(
                compact["Outputs"],
                ensure_ascii=False,
            )
        )
    else:
        lines.append("NONE")

    lines.append("")

    # --------------------------------------------------------
    # Components
    # --------------------------------------------------------

    lines.append("COMPONENTS")

    lines.append("-" * 60)

    stats = component_stats(
        compact["Components"]
    )

    for name, count in stats.most_common():

        lines.append(
            f"{count}x {name}"
        )

    lines.append("")

    # Detailed component index only.
    # No positions or visual data.

    lines.append(
        "COMPONENT INDEX"
    )

    lines.append("-" * 60)

    for component in compact[
        "Components"
    ]:

        parts = []

        if component["id"] is not None:
            parts.append(
                f"ID={component['id']}"
            )

        if component["name"] is not None:
            parts.append(
                f"NAME={component['name']}"
            )

        if component["type"] is not None:
            parts.append(
                f"TYPE={component['type']}"
            )

        if component["label"] not in (
            None,
            "",
        ):
            parts.append(
                f"LABEL={component['label']}"
            )

        lines.append(
            " | ".join(parts)
        )

    lines.append("")

    # --------------------------------------------------------
    # Connections
    # --------------------------------------------------------

    lines.append(
        "CONNECTIONS"
    )

    lines.append("-" * 60)

    for wire in compact[
        "Connections"
    ]:

        connection = wire[
            "connection"
        ]

        if (
            "from" in connection
            and "to" in connection
        ):

            source = json.dumps(
                connection["from"],
                ensure_ascii=False,
                separators=(",", ":"),
            )

            target = json.dumps(
                connection["to"],
                ensure_ascii=False,
                separators=(",", ":"),
            )

            lines.append(
                f"{source} -> {target}"
            )

        else:

            lines.append(
                json.dumps(
                    connection,
                    ensure_ascii=False,
                    separators=(",", ":"),
                )
            )

    lines.append("")

    # --------------------------------------------------------
    # Machine-readable compact JSON
    # --------------------------------------------------------

    lines.append(
        "COMPACT JSON"
    )

    lines.append("-" * 60)

    lines.append(
        json.dumps(
            compact,
            ensure_ascii=False,
            separators=(",", ":"),
        )
    )

    return "\n".join(lines)


# ============================================================
# Main
# ============================================================

def main():

    if len(sys.argv) != 2:

        print(
            "Usage:"
        )

        print(
            "  python3 dls_compact.py circuit.json"
        )

        sys.exit(1)

    input_path = Path(
        sys.argv[1]
    )

    if not input_path.exists():

        print(
            f"File not found: {input_path}"
        )

        sys.exit(1)

    # --------------------------------------------------------
    # Load
    # --------------------------------------------------------

    try:

        with input_path.open(
            "r",
            encoding="utf-8",
        ) as file:

            data = json.load(file)

    except json.JSONDecodeError as exc:

        print(
            f"Invalid JSON: {exc}"
        )

        sys.exit(1)

    # --------------------------------------------------------
    # Process
    # --------------------------------------------------------

    compact = build_compact(
        data
    )

    # --------------------------------------------------------
    # Output
    # --------------------------------------------------------

    json_path = (
        input_path.with_suffix(
            ".compact.json"
        )
    )

    txt_path = (
        input_path.with_suffix(
            ".compact.txt"
        )
    )

    with json_path.open(
        "w",
        encoding="utf-8",
    ) as file:

        json.dump(
            compact,
            file,
            ensure_ascii=False,
            separators=(",", ":"),
        )

        file.write("\n")

    prompt = make_prompt(
        compact
    )

    with txt_path.open(
        "w",
        encoding="utf-8",
    ) as file:

        file.write(prompt)
        file.write("\n")

    # --------------------------------------------------------
    # Statistics
    # --------------------------------------------------------

    original_size = (
        input_path.stat().st_size
    )

    compact_size = (
        json_path.stat().st_size
    )

    reduction = (
        1 -
        compact_size / original_size
    ) * 100 if original_size else 0

    print()
    print(
        "DLS compact extraction complete."
    )
    print()

    print(
        f"Original : {original_size:,} bytes"
    )

    print(
        f"Compact  : {compact_size:,} bytes"
    )

    print(
        f"Reduction: {reduction:.1f}%"
    )

    print()

    print(
        f"Components: "
        f"{len(compact['Components']):,}"
    )

    print(
        f"Connections: "
        f"{len(compact['Connections']):,}"
    )

    print()

    print(
        f"JSON: {json_path}"
    )

    print(
        f"Text: {txt_path}"
    )

    print()


if __name__ == "__main__":
    main()