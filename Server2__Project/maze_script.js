let generate_btn = document.getElementById("generate");

const MAZE_DIM = 10;                // Number of columns and rows
const COL_WIDTH = "50px ";          // Width of each column
let click_count = 0;
let ms_delay = 10;
let maze = null;
let coroutines = [];

function delay(ms) {
    // To-do: Learn why we can await a `Promise` object without providing an argument for `res` using `.then()`...
    return new Promise(function (res) {
        setTimeout(res, ms);
    });
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
        this.maze_container.style["grid-template-columns"] = this.col_width.repeat(this.maze_dim);
    }

    async maze_factory() {
        await this.construct_skeleton();
    }

    *generate() {
        for (let row = 0; row < this.maze_dim; row++) {
            let maze_cell_row = [];
            for (let col = 0; col < this.maze_dim; col++) {
                // create_cell();
                let maze_cell = new Maze_Cell(row, col);
                this.maze_container.appendChild(maze_cell.cell);
                maze_cell_row.push(maze_cell);
                yield;
            }
            this.#maze_cells.push(maze_cell_row);
        }
        yield;
    }

    destroy() {
        this.maze_container.innerHTML = "";
        this.#maze_cells = [];
    }
}

/**
 * Next steps:
 * 
 * Clean up game manager logic:
 * 
 * 1. Try to get rid of global variables.
 * 2. Ideally, we want a restartGame and beginGame, where restartGame stops, destroys, and calls beginGame.
 * 
 * Add generate maze: 1. start at random point; 2. take step in random direction (random direction function generates 0, 1, 2, 3 and returns an int vector)
 * Will need to have a visited list for backtracking.
 * Implement path finding or something ??
 */

function Coroutine(func) {
    this.run = true;
    coroutines.push(this);
    async function start_coroutine(func) {
        const f = func.bind(maze)();
        while (this.run) {
            if (f.next().done == true) {
                break;
            }
            await delay(ms_delay);
        }
        const index = coroutines.indexOf(this);
        coroutines.splice(index, 1);
    }
    const start = start_coroutine.bind(this);
    start(func);    
}

function stop_all_coroutines() {
    for (coroutine of coroutines) {
        coroutine.run = false;
    }
}

function begin_game() {
    maze = new Maze(MAZE_DIM, COL_WIDTH);
    const coroutine = new Coroutine(maze.generate);
}

function restart_game() {
    stop_all_coroutines();
    maze.destroy();
    begin_game();
}

function generate_btn_click() {
    click_count++;
    if (click_count == 1) {
        begin_game();
    }
    else {
        restart_game();
    }
}

generate_btn.addEventListener("click", generate_btn_click);