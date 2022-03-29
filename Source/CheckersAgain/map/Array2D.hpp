#pragma once

#include "../util.hpp"


template<class Cell>
class Array2D {
public:
	uint width;
	uint height;
	List<Cell> cells;

	Array2D()
		: width(0),
		  height(0),
		  cells()
	{}
	
	Array2D(uint width, uint height)
		: width(width),
		  height(height),
		  cells(width * height)
	{}

	fn reset(uint new_width, uint new_height) {
		width = new_width;
		height = new_height;
		cells = move(List<Cell>(new_width * new_height));
	}

	fn is_valid(int x, int y) const {
		return x >= 0 && y >= 0 && uint(x) < width && uint(y) < height;
	}

	fn get(int x, int y) -> Cell& {
		assuming (is_valid(x,y));
		return cells[x + y * width];
	}

	fn get(int x, int y) const -> const Cell& {
		assuming (is_valid(x,y));
		return cells[x + y * width];
	}

	fn begin() {
		return cells.begin();
	}

	fn end() {
		return cells.end();
	}
};
