from mawatam import yaml_spacer, format_square_glues
import copy


class Tileset(object):
    def __init__(self):
        # Mapping from glue name to glue strength
        self.glues = {}
        self.temperature = 2
        # Mapping from tile type name to tile type square glues
        self.tileset_tile_types = {}

    # Returns a copy of the tileset at a given temperature
    def at_temperature(self, temperature):
        to_return = Tileset()
        to_return.glues = copy.deepcopy(self.glues)
        to_return.tileset_tile_types = copy.deepcopy(self.tileset_tile_types)
        to_return.temperature = temperature

    def add_glue(self, glue_name, glue_temp):
        self.glues[glue_name] = glue_temp

    def add_tile_type(self, tile_type_name, tile_type_square_glues):
        self.tileset_tile_types[tile_type_name] = tile_type_square_glues[:]

    def __str__(self):
        to_return = f"temperature: {self.temperature}\n\n"
        to_return += "glues:\n"
        for glue in self.glues:
            to_return += (
                yaml_spacer + f"{glue}: {format_square_glues(self.glues[glue])}\n"
            )

        to_return += "\ntileset_tile_types:\n"
        for tile_type_name in self.tileset_tile_types:
            to_return += (
                yaml_spacer
                + f"{tile_type_name}: {format_square_glues(self.tileset_tile_types[tile_type_name])}\n"
            )
        return to_return
