let generate_btn = document.getElementById("generate");

const MAZE_DIM = 10;                // Number of columns and rows
const COL_WIDTH = "50px ";          // Width of each column

function delay(ms) {
    // To-do: Learn why we can await a `Promise` object without providing an argument for `res` using `.then()`...
    return new Promise(function (res) {
        setTimeout(res, ms);
    })
}

class Maze_Cell {
    constructor(row, col) {
        this.row = row;
        this.col = col;
        this.num = 10 * row + (col + 1);

        // Cell
        this.cell = document.createElement("div");
        this.cell.className = "maze_cell";

        // North wall
        this.n_wall = document.createElement("div");
        this.n_wall.className = "north_wall";
        this.cell.appendChild(this.n_wall);

        // West wall
        this.w_wall = document.createElement("div");
        this.w_wall.className = "west_wall";
        this.cell.appendChild(this.w_wall);

        // Cell number
        this.cell_number = document.createElement("div");
        this.cell_number.className = "center_div";
        this.cell_number.innerHTML = this.num;
        this.cell.appendChild(this.cell_number);

        // East wall
        this.e_wall = document.createElement("div");
        this.e_wall.className = "east_wall";
        this.cell.appendChild(this.e_wall);

        // South wall
        this.s_wall = document.createElement("div");
        this.s_wall.className = "south_wall";
        this.cell.appendChild(this.s_wall);
        
        
    }
}

class Maze {
    #maze_cells = [];
    constructor(maze_dim, col_width) {
        this.maze_dim = maze_dim;
        this.col_width = col_width;
        this.maze_container = document.getElementsByClassName("maze_container")[0];
        this.maze_factory();
    }

    async maze_factory() {
        await this.construct_skeleton();
    }

    async construct_skeleton() {
        this.maze_container.style["grid-template-columns"] = this.col_width.repeat(this.maze_dim);
        for (let row = 0; row < this.maze_dim; row++) {
            let maze_cell_row = [];
            for (let col = 0; col < this.maze_dim; col++) {
                let maze_cell = new Maze_Cell(row, col);
                this.maze_container.appendChild(maze_cell.cell);
                maze_cell_row.push(maze_cell);
                await delay(10);
            }
            this.#maze_cells.push(maze_cell_row);
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

function generate_btn_click() {
    const maze = new Maze(MAZE_DIM, COL_WIDTH);
}

let click_count = 0;
let stop_flag = false;

function* test() {
    console.log("test1");
    yield;
    console.log("test2");
    yield;
    console.log("test3");
}

async function start_coroutine(f) {
    var func = f();
    var run_counter = 0;
    while (stop_flag == false && run_counter < 20) {
        func.next();
        run_counter += 1;
        await delay(10);
        console.log(run_counter);
    }
}

function generate_btn_click_p() {
    click_count += 1;

    // new maze

    if (click_count == 1) {
        // start_coroutine(maze.generate)
        start_coroutine(test);
    }
    else {
        stop_flag = true;
        // destroy `maze.maze_container`'s children
        // start_coroutine(maze.generate)
    }
}

generate_btn.addEventListener("click", generate_btn_click);