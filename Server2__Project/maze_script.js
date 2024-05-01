let generate_btn = document.getElementById("generate");
let maze_container = document.getElementsByClassName("maze_container")[0];

// Maze has `maze_dim` * `maze_dim` cells.
const maze_dim = 10;
const col_width = "50px ";
maze_container.style["grid-template-columns"] = col_width.repeat(maze_dim);

/**
 * Next steps:
 * 
 * Create matrix to hold maze cells.
 * Create maze cell walls.
 * Create game manager.
 * Possible classes: Maze, MazeCell
 */

function create_maze_cell(row, col) {
    const maze_cell = document.createElement("div");
    maze_cell.className = "maze_cell";
    maze_cell.innerHTML = 10 * row + (col + 1);
    maze_container.appendChild(maze_cell);
}

function delay(ms) {
    // To-do: Learn why we can await a `Promise` object without providing an argument for `res` using `.then()`...
    return new Promise(function (res) {
        setTimeout(res, ms);
    })
}

async function generate_btn_click() {
    for (let row = 0; row < maze_dim; row++) {
        for (let col = 0; col < maze_dim; col++) {
            create_maze_cell(row, col);
            await delay(50);
        }
    }
}

generate_btn.addEventListener("click", generate_btn_click);