import tkinter as tk
from tkinter import filedialog
import subprocess
import time
import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# Función para ejecutar un programa de C++
def run_cpp_program(program_path, input_file, output_file, k, index_query=0):
    command = [program_path, str(k), input_file, index_query]
    start_time = time.time()
    with open(output_file, 'w') as file:
        subprocess.run(command, check=True, stdout=file, stderr=subprocess.PIPE)
    execution_time = time.time() - start_time
    return execution_time

def graficar_serie_temporal(layout, serie_temporal, color, algorithm, k):
    #fig = go.Figure()
    row, col = 1, 1
    if algorithm == 'ParSketch':
        row, col = 1, 1
    elif algorithm == 'isax':
        row, col = 2, 1

    layout.add_trace(go.Scatter(x=list(range(1, len(serie_temporal[0]) + 1)), y=serie_temporal[0], 
                             mode='lines+markers', name=f"Query",
                             line=dict(color='black', width=4, dash='dash')),
                             row=row, col=col)

    for i, serie_tiempo in enumerate(serie_temporal[1:]):
        layout.add_trace(go.Scatter(x=list(range(1, len(serie_tiempo) + 1)), y=serie_tiempo, 
                                    mode='lines+markers', name=f"Candidato {i + 1} {algorithm}"),
                                    row=row, col=col)

        
# Función para seleccionar un archivo de series temporales y plotearlo
def select_file_and_plot():
    file_path = filedialog.askopenfilename(title="Seleccione un archivo de series temporales")
    if not file_path:
        return
    
     # Obtener el valor de k del usuario
    k = tk.simpledialog.askinteger("Input", "Ingrese el valor de k:", parent=root, minvalue=1)
    index_query = tk.simpledialog.askinteger("Input", "Indice de la query:", parent=root, minvalue=1)


    # Ejecutar programas de C++ y medir el tiempo de ejecución
    cpp_program1 = './omp'  # Reemplaza con la ruta correcta de tu primer programa
    cpp_program2 = './programa2'  # Reemplaza con la ruta correcta de tu segundo programa

    output_file1 = 'par-sketch-results.txt'
    output_file2 = 'resultados_programa2.txt'

    execution_time1 = run_cpp_program(cpp_program1, file_path, output_file1, k, index_query)
    execution_time2 = 0
    #execution_time2 = run_cpp_program(cpp_program2, file_path, output_file2, k)

    print(f'Tiempo de ejecución del ParSketch: {execution_time1} segundos')
    #print(f'Tiempo de ejecución del programa 2: {execution_time2} segundos')

    # Leer resultados de los archivos generados por los programas de C++
    with open(output_file1, 'r') as file:
        results1 = [[float(num) for num in line.strip().split()] for line in file]

    results2 = [[]]
    #with open(output_file2, 'r') as file:
    #    results2 = [[float(num) for num in line.strip().split()] for line in file]
 
    fig = make_subplots(rows=3, cols=1,
                    shared_xaxes=True,
                    vertical_spacing=0.02,
                    subplot_titles=("ParSketch", "iSax", "Query"))

    fig.update_layout(
            title=f'Resultado de busqueda de KNN ParSketch vs iSax',
            #xaxis=dict(title='Índice del Punto en la Serie de Tiempo'),
            yaxis=dict(title='Data points'),
            legend=dict(title='Series de Tiempo')
        )
    graficar_serie_temporal(fig, results1, 'red', 'ParSketch', k)

    fig.add_annotation( 
        text=f'Tiempo de ejecución de ParSketch: {execution_time1:.4f} segundos\n',
        xref='paper', yref='paper',
        x=0.05, y=0.95,
        showarrow=False,
        font=dict(size=12)
    )
    fig.add_annotation( 
        text=f'Tiempo de ejecución de iSax: {execution_time2:.4f} segundos\n',
        xref='paper', yref='paper',
        x=0.05, y=0.5,
        showarrow=False,
        font=dict(size=12)
    )
    fig.add_trace(go.Scatter(x=list(range(1, len(results1[0]) + 1)), y=results1[0], 
                        mode='lines+markers', name=f"Query",
                        line=dict(color='black')),
                        row=3, col=1
                ) 
    fig.show()


# Crear la interfaz de usuario
root = tk.Tk()
root.title("Selección y Plot de Series Temporales")

button = tk.Button(root, text="Seleccionar Archivo y Plotear", command=select_file_and_plot)
button.pack(pady=20)

root.mainloop()
