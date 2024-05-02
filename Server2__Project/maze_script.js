let generate_btn = document.getElementById("generate");
let maze_container = document.getElementsByClassName("maze_container")[0];

// Maze has `maze_dim` * `maze_dim` cells.
const maze_dim = 10;
const col_width = "50px ";
maze_container.style["grid-template-columns"] = col_width.repeat(maze_dim);
maze_cells = [];

class Maze_Cell {
    constructor(row, col) {
        this.row = row;
        this.col = col;
        this.num = 10 * row + (col + 1);
        this.cell = document.createElement("div");
        this.cell.className = "maze_cell";
        this.cell.innerHTML = this.num;
        this.n_wall = document.createElement("div");
        this.n_wall.className = "north_wall";
        this.cell.appendChild(this.n_wall);
        this.s_wall = document.createElement("div");
        this.s_wall.className = "south_wall";
        this.cell.appendChild(this.s_wall);
        this.w_wall = document.createElement("div");
        this.w_wall.className = "west_wall";
        this.cell.appendChild(this.w_wall);
        this.e_wall = document.createElement("div");
        this.e_wall.className = "east_wall";
        this.cell.appendChild(this.e_wall);
    }
}

class Maze {
    #maze_cells = [];
    constructor(maze_dim, col_width) {
        this.maze_dim = maze_dim;
        this.col_width = col_width;
        this.maze_container = document.getElementByClassName("maze_container")[0];
        this.construct_skeleton();
    }

    construct_skeleton() {
        this.maze_container.style["grid-template-columns"] = this.col_width.repeat(this.maze_dim);
        for (let row = 0; row < this.maze_dim; row++) {
            let maze_cell_row = [];
            for (let col = 0; col < this.maze_dim; col++) {
                let maze_cell = new Maze_Cell(row, col);
                this.maze_container.appendChild(maze_cell.cell);
                maze_cell_row.push(maze_cell);
            }
            maze_cells.push(maze_cell_row);
        }
    }
}

/**
 * Next steps:
 * 
 * Test classes.
 * Create game manager.
 * Add generate maze: 1. start at random point; 2. take step in random direction (random direction function generates 0, 1, 2, 3 and returns an int vector)
 * Will need to have a visited list for backtracking.
 * Implement path finding or something ??
 */

function create_maze_cell(row, col) {
    const maze_cell = document.createElement("div");
    maze_cell.className = "maze_cell";
    maze_cell.innerHTML = 10 * row + (col + 1);
    maze_container.appendChild(maze_cell);
    return maze_cell;
}

function delay(ms) {
    // To-do: Learn why we can await a `Promise` object without providing an argument for `res` using `.then()`...
    return new Promise(function (res) {
        setTimeout(res, ms);
    })
}

async function generate_btn_click() {
    for (let row = 0; row < maze_dim; row++) {
        let maze_cell_row = []
        for (let col = 0; col < maze_dim; col++) {
            maze_cell = create_maze_cell(row, col);
            maze_cell_row.push(maze_cell);
            await delay(10);
        }
        maze_cells.push(maze_cell_row);
    }
}

generate_btn.addEventListener("click", generate_btn_click);