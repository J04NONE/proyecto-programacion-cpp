# Manual de Uso — Clasificador BreaKHis
### Para: Iván Velasco · Joan Murillo
### Presentación: 2026-05-23

---

## 0. Qué hace este sistema

Clasifica imágenes histopatológicas de tejido mamario en **Benigno** o **Maligno** usando una red neuronal ResNet50 con Transfer Learning. Todo corre en Google Colab con GPU gratuita.

**Tiempo total de ejecución:** ~10–15 minutos (con GPU T4).

---

## 1. Preparación (hacer ANTES de la presentación)

### 1.1. Cuenta de Kaggle (una sola vez, ya configurada idealmente)

El dataset BreaKHis se descarga desde Kaggle. Necesitas credenciales:

1. Entra a https://www.kaggle.com → tu perfil → **Settings**
2. Baja a la sección **API** → **Create New Token**
3. Se descarga `kaggle.json` con este formato:
   ```json
   {"username": "tu_usuario", "key": "tu_api_key_largo"}
   ```
4. Guarda este archivo — lo subirás a Colab más tarde.

### 1.2. Abrir el notebook en Google Colab

1. Ve a https://colab.research.google.com/
2. **Archivo → Subir notebook** → selecciona `breakhis_colab.ipynb`
3. El notebook se abre con todas las celdas listas para ejecutar.

### 1.3. Activar la GPU

**CRÍTICO** — sin GPU, el entrenamiento tarda 10x más.

1. Menú **Entorno de ejecución → Cambiar tipo de entorno de ejecución**
2. En "Acelerador de hardware" selecciona **T4 GPU**
3. Click en **Guardar**

### 1.4. Subir credenciales de Kaggle a Colab

En el panel lateral izquierdo de Colab:
1. Click en el icono de **carpeta**
2. Click en **Subir** (icono de hoja con flecha hacia arriba)
3. Sube tu archivo `kaggle.json`

> Si prefieres no subir el archivo, puedes editar la celda 5 y descomentar las líneas:
> ```python
> os.environ['KAGGLE_USERNAME'] = 'TU_USUARIO'
> os.environ['KAGGLE_KEY'] = 'TU_API_KEY'
> ```

---

## 2. Ejecución paso a paso

### Recomendación: ejecutar TODO antes de la presentación

Menú **Entorno de ejecución → Ejecutar todas**. Esto demora ~10–15 min y deja todas las gráficas y métricas listas.

### Si prefieres ir celda por celda:

| # | Celda | Qué hace | Tiempo aprox. |
|---|-------|----------|---------------|
| 1 | Markdown | Portada | — |
| 2 | `!pip install` | Instala dependencias | 30 s |
| 3 | imports + DEVICE | Detecta GPU y semilla | 5 s |
| 4 | Markdown | — | — |
| 5 | `kagglehub.dataset_download` | Descarga dataset (~3 GB) | 1–3 min |
| 6 | Definir rutas | Verifica que Folds.csv existe | 1 s |
| 7 | **Verificación** | `assert` que las rutas son válidas | 1 s |
| 8 | Filtrar fold 1, 40X | Crea train_df y val_df | 1 s |
| 9 | Distribución clases | Gráfica de barras | 2 s |
| 10 | Muestras del dataset | Visualiza 8 imágenes | 5 s |
| 11 | Markdown | — | — |
| 12 | Dataset + DataLoader | Define pipeline + pos_weight | 1 s |
| 13 | Markdown | — | — |
| 14 | `create_model` | Carga ResNet50 ImageNet | 30 s (primera vez) |
| 15 | Markdown | — | — |
| 16 | `train_epoch` + `evaluate` | Define funciones | 1 s |
| 17 | **FASE 1 — Head only** | Entrena 5 épocas | ~2 min |
| 18 | **FASE 2 — Fine tuning** | Entrena 5 épocas más | ~4 min |
| 19 | Markdown | — | — |
| 20 | Curvas de entrenamiento | Loss y F1 por época | 2 s |
| 21 | Markdown | — | — |
| 22 | Evaluación final | F1, Recall, AUC | 30 s |
| 23 | Matriz de confusión | Heatmap con TN/FP/FN/TP | 2 s |
| 24 | Markdown | — | — |
| 25 | Predicciones individuales | 8 ejemplos con probabilidad | 10 s |
| 26 | Markdown | Resumen | — |

---

## 3. Qué decir en cada parte (resumen del guión)

### Iván — celdas 5 a 10
- Mientras carga el dataset (celda 5): habla del contexto clínico
- Celda 9 (distribución): "Aquí vemos el desbalance — 70% maligno, 30% benigno"
- Celda 10 (muestras): "Estas son las imágenes con tinción H&E que ve el patólogo"

### Joan — celdas 12 a 25
- Celda 12: "Dataset + DataLoader usando los folds oficiales para patient-level split"
- Celda 14: "ResNet50 preentrenado en ImageNet — solo entrenamos la última capa"
- Celdas 17 y 18: "Fase 1 entrena solo la cabeza, Fase 2 hace fine-tuning"
- Celda 20: "Curvas — la línea vertical marca el inicio de fine-tuning"
- Celda 23: "**Matriz de confusión** — los Falsos Negativos son lo más crítico clínicamente"
- Celda 25: "Predicciones individuales con la probabilidad del modelo"

---

## 4. Solución de problemas

| Problema | Causa probable | Solución |
|----------|----------------|----------|
| `Dispositivo: cpu` | No activaste la GPU | Runtime → Change runtime type → T4 GPU |
| `KaggleApiException` | Credenciales no configuradas | Subir `kaggle.json` (sección 1.4) |
| `AssertionError: IMAGES_BASE no existe` | Estructura del dataset cambió | Verifica `!ls {DATASET_PATH}` en celda 5 |
| `CUDA out of memory` | Batch muy grande | Reducir `BATCH_SIZE` a 16 en celda 12 |
| Notebook congelado en DataLoader | Deadlock multiprocessing | Ya está fijado: `num_workers=0` |
| `roc_auc_score ValueError` | Solo una clase en val | No debería pasar — verificar Folds.csv |
| F1 muy bajo (<0.5) | Modelo no convergió | Ejecutar más épocas o revisar pos_weight |

---

## 5. Backup — si Colab falla en plena presentación

Después de la ejecución completa, Colab habrá guardado estos archivos en el panel lateral:

- `distribucion_clases.png`
- `muestras_dataset.png`
- `curvas_entrenamiento.png`
- `confusion_matrix.png`
- `predicciones_ejemplo.png`
- `breakhis_resnet50_best.pth`

**Descárgalos TODOS a tu laptop antes de la presentación** (click derecho → Download). Si Colab cae durante la expo, tienes las gráficas como backup en PowerPoint.

---

## 6. Checklist final — día de la presentación

### El día antes (2026-05-22 — hoy)
- [ ] Ejecutar el notebook completo en Colab
- [ ] Descargar las 5 imágenes PNG generadas
- [ ] Descargar `breakhis_resnet50_best.pth`
- [ ] Anotar las métricas finales (F1, Recall, AUC) para citarlas en la conclusión
- [ ] Ensayar el guión una vez con Iván

### El día de la presentación
- [ ] Llegar 15 min antes
- [ ] Abrir Colab → re-ejecutar celdas 1 a 17 (~7 min) para tener todo cacheado
- [ ] Verificar que la GPU sigue activa (re-conectar si se desconectó)
- [ ] Tener las PNGs y el PDF de backup abiertos en otra pestaña
- [ ] Tener el guión `guion_exposicion.md` impreso o en otra pantalla

---

## 7. División de responsabilidades

| Responsabilidad | Quién |
|-----------------|-------|
| Configurar Kaggle y subir credenciales | Joan |
| Abrir Colab y verificar GPU | Joan |
| Ejecutar el notebook | Joan |
| Hablar de las celdas 5–10 (contexto + dataset) | Iván |
| Hablar de las celdas 12–25 (técnica + demo) | Joan |
| Conclusiones y métricas finales | Ambos |
| Responder preguntas técnicas | Joan |
| Responder preguntas clínicas | Iván |

---

## 8. Después de la presentación

Si quieren extender el sistema más allá del entregable:
- **Ensemble ResNet50 + DenseNet121** → +2-3% F1
- **Entrenamiento en los 5 folds** → métricas con desviación estándar (más rigurosas)
- **FastAPI** → exponer el modelo como servicio web
- **Múltiples magnificaciones** → train independiente por magnificación + voting

Todo esto está detallado en `docs/superpowers/specs/2026-05-16-breakhis-classifier-design.md`.

---

**Contacto técnico si algo se rompe:** Joan
**Contacto clínico si preguntan algo médico:** Iván

Suerte mañana.
