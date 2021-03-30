from datam import yaml_spacer, format_pos, format_square_glues


class Configuration(object):
    def __init__(self, tileset):
        self.tileset = tileset
        self.tiles = {}
        self.input_sides = {}

    # `input_side` is a list that let you specify
    # which side of the tile are used to give input to the system
    # it is useful to have this information when composing
    # different configuration together (we want to mask inputs in
    # that case)
    def add_tile(self, position, glues, input_sides=[]):
        self.tiles[position] = glues[:]
        self.input_sides[position] = input_sides[:]

    # Returns a copy of the configuration where
    # input sides are masked
    def mask_input(self):
        new_config = Configuration(self.tileset)
        for position in self.tiles:
            new_config.input_sides[position] = input_sides[position][:]
            new_config.tiles[position] = tiles[position][:]
            for in_side in new_config.input_sides[position]:
                new_config.tiles[position][in_side] = None

        return new_config

    # Return a translated version of the configuration
    def translate(self, t_x, t_y):
        new_config = Configuration(self.tileset)
        for position in self.tiles:
            translated_position = (position[0] + t_x, position[1] + t_y)
            new_config.input_sides[translated_position] = input_sides[position][:]
            new_config.tiles[translated_position] = tiles[position][:]
        return new_config

    def __str__(self):
        to_return = str(self.tileset) + "\n"
        to_return += "configuration:\n"
        for position in self.tiles:
            to_return += yaml_spacer + format_pos(position) + ": "
            to_return += format_square_glues(self.tiles[position]) + "\n"

        return to_return
