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

function Int_vector(x, y) {
    this.x = x;
    this.y = y;
    this.add = function (v) {
        const x_new = v.x + this.x;
        const y_new = v.y + this.y;
        const v_new = new Int_vector(x_new, y_new);
        return v_new;
    };
}

// Turn matrix into Cartesian coordinate system with origin at the M_{00}
// +x-axis points South
// +y-axis points East
const directions = {
    North: new Int_vector(-1, 0),
    South: new Int_vector(1, 0),
    East: new Int_vector(0, 1),
    West: new Int_vector(0, -1)
};

class Maze_Cell {
    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.position = new Int_vector(x, y);
        this.visited = false;
        this.num = 10 * x + (y + 1);

        // Create cell:
        this.cell = document.createElement("div");
        const cell = this.cell;
        this.cell.className = "maze_cell";

        // Create walls:
        this.walls = {
            north: document.createElement("div"),
            west: document.createElement("div"),
            east: document.createElement("div"),
            south: document.createElement("div")
        };

        const wall_names = [];
        for (const wall_name in this.walls) {
            wall_names.push(wall_name);
        }
        const wall_classes = ["north_wall", "west_wall", "east_wall", "south_wall"];
        const wall_names_classes = wall_names.map(function (e, i) {
            return [e, wall_classes[i]];
        });

        for (const [wall_name, wall_class] of wall_names_classes) {
            let wall = this.walls[wall_name];
            wall.className = wall_class;
            this.cell.appendChild(wall);
        }

        // Create cell number:
        this.cell_number = document.createElement("div");
        this.cell_number.className = "center_div";
        this.cell_number.innerHTML = this.num;
        this.cell.insertBefore(this.cell_number, this.walls["east"]);
    }
}

class Maze {
    #maze_cells = [];
    #active_cells = [];
    #visited = [];
    constructor(maze_dim, col_width) {
        this.maze_dim = maze_dim;
        this.col_width = col_width;
        this.maze_container = document.getElementsByClassName("maze_container")[0];
        this.maze_container.style["grid-template-columns"] = this.col_width.repeat(this.maze_dim);
    }

    *generate_skeleton() {
        for (let row = 0; row < this.maze_dim; row++) {
            let maze_cell_row = [];
            let visited_row = [];
            for (let col = 0; col < this.maze_dim; col++) {
                // create_cell();
                let maze_cell = new Maze_Cell(row, col);
                this.maze_container.appendChild(maze_cell.cell);
                maze_cell_row.push(maze_cell);
                visited_row.push(false);
                yield;
            }
            this.#maze_cells.push(maze_cell_row);
            this.#visited.push(visited_row);
        }
        
        yield;
    }

    *generate_maze() {
        for (let row = 0; row < this.maze_dim; row++) {
            for (let col = 0; col < this.maze_dim; col++) {
                let current_cell = this.#maze_cells[row][col];
                current_cell.cell_number.innerHTML = "";
                for (const [wall_name, wall] of Object.entries(current_cell.walls)) {
                    wall.style.display = "none";
                }
            }
        }

        const rand_coordinates = this.get_random_coordinates();
        const active_cell = this.get_cell(rand_coordinates);
        active_cell.visited = true;
        this.#visited[active_cell.position.x][active_cell.position.y] = true;
        active_cell.cell.style.background = "red";
        this.#active_cells.push(active_cell);
        yield;        

        const random_neighbor = this.get_random_neighbor(active_cell);

        yield;
    }

    destroy() {
        this.maze_container.innerHTML = "";
        this.#maze_cells = [];
    }

    get_random_coordinates() {
        const max = this.maze_dim;
        const x = Math.floor(Math.random() * max);
        const y = Math.floor(Math.random() * max);
        return new Int_vector(x, y);
    }

    contains_coordinates(coordinates) {
        return (0 <= coordinates.x) && (coordinates.x < this.maze_dim) && (0 <= coordinates.y) && (coordinates.y < this.maze_dim);
    }

    get_cell(coordinates) {
        return this.#maze_cells[coordinates.x][coordinates.y];
    }

    get_random_neighbor(active_cell) {
        let possible_neighbors = [];

        for (const [direction, direction_vector] of Object.entries(directions)) {
            const neighbor_coordinates = active_cell.position.add(direction_vector);
            if (this.contains_coordinates(neighbor_coordinates)) {
                if (!this.#visited[neighbor_coordinates.x][neighbor_coordinates.y]) {
                    const neighbor = this.get_cell(neighbor_coordinates);
                    possible_neighbors.push(neighbor);
                }
            }
        }

        const num_poss_neighbors = possible_neighbors.length;
        if (num_poss_neighbors == 0) {
            return null;
        }
        else {
            return possible_neighbors[Math.floor(Math.random() * num_poss_neighbors)];
        }
    }

    build_passage(active_cell, neighbor) {
        //
    }
}

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
    const x = start(func);
    return x;
}

function stop_all_coroutines() {
    for (coroutine of coroutines) {
        coroutine.run = false;
    }
}

async function begin_game() {
    maze = new Maze(MAZE_DIM, COL_WIDTH);
    const coroutine_skel = new Coroutine(maze.generate_skeleton);
    await coroutine_skel;
    const coroutine_maze = new Coroutine(maze.generate_maze);
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