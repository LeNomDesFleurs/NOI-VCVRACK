#!/usr/bin/env python3

import sys
import os
import re
import json
import xml.etree.ElementTree


# Version check
f"Python 3.6+ is required"


class UserException(Exception):
    pass


def eprint(*args, **kwargs):
	print(*args, file=sys.stderr, **kwargs)


def find(f, array):
	for a in array:
		if f(a):
			return f

def input_default(prompt, default=""):
	str = input(f"{prompt} [{default}]: ")
	if str == "":
		return default
	return str


def is_valid_slug(slug):
	return re.match(r'^[a-zA-Z0-9_\-]+$', slug) != None


def str_to_identifier(s):
	if not s:
		return "_"
	# Identifiers can't start with a number
	if s[0].isdigit():
		s = "_" + s
	# Capitalize first letter
	s = s[0].upper() + s[1:]
	# Replace special characters with underscore
	s = re.sub(r'\W', '_', s)
	return s


def create_module(slug, panel_filename=None, source_filename=None):
	# Check slug
	if not is_valid_slug(slug):
		raise UserException("Slug must only contain ASCII letters, numbers, '-', and '_'.")

	# Check filenames
	if panel_filename:
		if not os.path.exists(panel_filename):
			raise UserException(f"Panel not found at {panel_filename}.")

		if source_filename and os.path.exists(source_filename):
			if input_default(f"{source_filename} already exists. Overwrite? (y/n)", "n").lower() != "y":
				return

		# Read SVG XML
		tree = xml.etree.ElementTree.parse(panel_filename)

		components = panel_to_components(tree)

		# Tell user to add model to plugin.hpp and plugin.cpp
		identifier = str_to_identifier(slug)


		# Write source
		source = components_to_source(components, slug)

		if source_filename:
			with open(source_filename, "w") as f:
				f.write(source)
			eprint(f"Source file generated at {source_filename}")
		else:
			print(source)


def panel_to_components(tree):
	ns = {
		"svg": "http://www.w3.org/2000/svg",
		"inkscape": "http://www.inkscape.org/namespaces/inkscape",
	}

	root = tree.getroot()
	# Get SVG scale relative to mm
	root_height = root.get('height')
	if root_height.endswith("mm"):
		scale = 1
	else:
		svg_dpi = 75
		mm_per_in = 25.4
		scale = mm_per_in / svg_dpi

	# Get components layer
	group = root.find(".//svg:g[@inkscape:label='components']", ns)
	# Illustrator uses `data-name` (in Unique object ID mode) or `id` (in Layer Names object ID mode) for the group name.
	# Don't test with `not group` since Elements with no subelements are falsy.
	if group is None:
		group = root.find(".//svg:g[@data-name='components']", ns)
	if group is None:
		group = root.find(".//svg:g[@id='components']", ns)
	if group is None:
		raise UserException("Could not find \"components\" layer on panel")

	components = {}
	components['params'] = []
	components['inputs'] = []
	components['outputs'] = []
	components['lights'] = []
	components['widgets'] = []

	for el in group:
		c = {}

		# Get name
		name = el.get('{' + ns['inkscape'] + '}label')
		# Illustrator names
		if not name:
			name = el.get('data-name')
		if not name:
			name = el.get('id')
		if not name:
			name = ""
		# Split name and component class name
		names = name.split('#', 1)
		name = names[0]
		if len(names) >= 2:
			c['cls'] = names[1]
		name = str_to_identifier(name).upper()
		c['name'] = name

		# Get position
		if el.tag == '{' + ns['svg'] + '}rect':
			x = float(el.get('x')) * scale
			y = float(el.get('y')) * scale
			width = float(el.get('width')) * scale
			height = float(el.get('height')) * scale
			c['x'] = round(x, 3)
			c['y'] = round(y, 3)
			c['width'] = round(width, 3)
			c['height'] = round(height, 3)
			c['cx'] = round(x + width / 2, 3)
			c['cy'] = round(y + height / 2, 3)
		elif el.tag == '{' + ns['svg'] + '}circle' or el.tag == '{' + ns['svg'] + '}ellipse':
			cx = float(el.get('cx')) * scale
			cy = float(el.get('cy')) * scale
			c['cx'] = round(cx, 3)
			c['cy'] = round(cy, 3)
		else:
			eprint(f"Element in components layer is not rect, circle, or ellipse: {el}")
			continue

		# Get color
		color = None
		# Get color from fill attribute
		fill = el.get('fill')
		if fill:
			color = fill
		# Get color from CSS fill style
		if not color:
			style = el.get('style')
			if style:
				color_match = re.search(r'fill:\S*(#[0-9a-fA-F]{6})', style)
				color = color_match.group(1)
		if not color:
			eprint(f"Cannot get color of component: {el}")
			continue

		color = color.lower()

		if color == '#ff0000' or color == '#f00' or color == 'red':
			components['params'].append(c)
		if color == '#00ff00' or color == '#0f0' or color == 'lime':
			components['inputs'].append(c)
		if color == '#0000ff' or color == '#00f' or color == 'blue':
			components['outputs'].append(c)
		if color == '#ff00ff' or color == '#f0f' or color == 'magenta':
			components['lights'].append(c)
		if color == '#ffff00' or color == '#ff0' or color == 'yellow':
			components['widgets'].append(c)

	# Sort components
	top_left_sort = lambda w: w['cy'] + 0.01 * w['cx']
	components['params'] = sorted(components['params'], key=top_left_sort)
	components['inputs'] = sorted(components['inputs'], key=top_left_sort)
	components['outputs'] = sorted(components['outputs'], key=top_left_sort)
	components['lights'] = sorted(components['lights'], key=top_left_sort)
	components['widgets'] = sorted(components['widgets'], key=top_left_sort)

	eprint(f"Found {len(components['params'])} params, {len(components['inputs'])} inputs, {len(components['outputs'])} outputs, {len(components['lights'])} lights, and {len(components['widgets'])} custom widgets in \"components\" layer.")
	return components


def components_to_source(components, slug):
	identifier = str_to_identifier(slug)
	source = ""
	
	# Params
	if len(components['params']) > 0:
		for c in components['params']:
			source += "\n"
			source += f"float {c['name']}_PARAM[2] = \u007b {c['cx']}, {c['cy']} \u007d;"
	# Inputs
	source += "\n"
	if len(components['inputs']) > 0:
		for c in components['inputs']:
			source += "\n"
			source += f"float {c['name']}_INPUT[2] = \u007b {c['cx']}, {c['cy']} \u007d;"
	# Outputs
	source += "\n"
	if len(components['outputs']) > 0:
		for c in components['outputs']:
			source += "\n"
			source += f"float {c['name']}_OUTPUT[2] = \u007b {c['cx']}, {c['cy']} \u007d;"
	# Lights
	source += "\n"
	if len(components['lights']) > 0:
		for c in components['lights']:
			source += "\n"
			source += f"float {c['name']}_LIGHT[2] = \u007b {c['cx']}, {c['cy']} \u007d;"

	# Widgets
	if len(components['widgets']) > 0:
		source += "\n"
	for c in components['widgets']:
		if 'x' in c:
			source += f"""
		// mm2px(Vec({c['width']}, {c['height']}))
		addChild(createWidget<{c.get('cls', 'Widget')}>(mm2px(Vec({c['x']}, {c['y']}))));"""
		else:
			source += f"""
		addChild(createWidgetCentered<{c.get('cls', 'Widget')}>(mm2px(Vec({c['cx']}, {c['cy']}))));"""


	return source


def usage(script):
	text = f"""strip down version of helper.py for 
retrieving component position from svg file"""
	eprint(text)


def parse_args(args):
	script = args.pop(0)
	if len(args) == 0:
		usage(script)
		return

	cmd = args.pop(0)
	if cmd == 'createplugin':
		create_plugin(*args)
	elif cmd == 'createmodule':
		create_module(*args)
	elif cmd == 'createmanifest':
		create_manifest(*args)
	else:
		eprint(f"Command not found: {cmd}")


if __name__ == "__main__":
	try:
		parse_args(sys.argv)
	except KeyboardInterrupt:
		pass
	except UserException as e:
		eprint(e)
		sys.exit(1)



		# Write manifest
	#	with open(manifest_filename, "w") as f:
	#		json.dump(manifest, f, indent="  ")

	#	eprint(f"Added {slug} to {manifest_filename}")