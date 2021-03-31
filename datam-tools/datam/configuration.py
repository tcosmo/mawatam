from datam import yaml_spacer, format_pos, format_square_glues, C
import sys


class Configuration(object):
    def __init__(self, tileset):
        self.tileset = tileset
        self.tiles = {}
        self.last_tile_pos = None

    def add_tile(self, position, glues=[None, None, None, None]):
        if not isinstance(position, C):
            position = C(position)
        self.tiles[position] = glues[:]
        self.last_tile_pos = position
        return self

    def delete_tile(self, position):
        if not isinstance(position, C):
            position = C(position)
        if position in self.tiles:
            del self.tiles[position]
        return self

    def add_glue(self, position):
        if not isinstance(position, C):
            position = C(position)
        self.last_tile_pos = position
        if position not in self.tiles:
            self.tiles[position] = [None, None, None, None]
        return self

    def north(self, glue):
        if self.last_tile_pos is None:
            return
        self.tiles[self.last_tile_pos][0] = glue
        return self

    def east(self, glue):
        if self.last_tile_pos is None:
            return
        self.tiles[self.last_tile_pos][1] = glue
        return self

    def south(self, glue):
        if self.last_tile_pos is None:
            return
        self.tiles[self.last_tile_pos][2] = glue
        return self

    def west(self, glue):
        if self.last_tile_pos is None:
            return
        self.tiles[self.last_tile_pos][3] = glue
        return self

    def add_sub_conf(self, sub_conf):
        for position in sub_conf.tiles:
            self.tiles[position] = sub_conf.tiles[position][:]
        return self

    # Return a translated version of the configuration
    def translate_aux(self, t_x, t_y):
        new_config = Configuration(self.tileset)
        for position in self.tiles:
            translated_position = C(position[0] + t_x, position[1] + t_y)
            new_config.tiles[translated_position] = self.tiles[position][:]
        return new_config

    def translate(self, t_vec):
        return self.translate_aux(t_vec[0], t_vec[1])

    # Return the bottom-most leftmost position
    # of the rectangle that fits the configuration
    def bottom_left(self):
        return C(
            min([pos.t()[0] for pos in self.tiles]),
            min([pos.t()[1] for pos in self.tiles]),
        )

    def __str__(self):
        to_return = str(self.tileset) + "\n"
        to_return += "configuration:\n"
        for position in self.tiles:
            to_return += yaml_spacer + format_pos(position) + ": "
            to_return += format_square_glues(self.tiles[position]) + "\n"

        return to_return
