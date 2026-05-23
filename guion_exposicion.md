# Guión de Exposición — BreaKHis Classifier
### Computación Científica · 2026-05-23
### Iván Velasco · Joan Murillo

> **Duración:** 15-18 minutos + preguntas
> **Formato:** Presentación con demo en vivo en Google Colab
> **Nota honesta:** ninguno de nosotros es médico, así que cuando hablemos del lado clínico vamos a citar lo que dice el paper original y no inventar.

---

## ÍNDICE
1. Parte 1 — Iván (contexto y dataset)
2. Parte 2 — Joan (solución técnica y demo)
3. Parte 3 — Ambos (conclusiones)
4. Glosario (términos médicos y técnicos)
5. Preguntas anticipadas del profesor (con respuestas honestas)

---

## PARTE 1 — IVÁN (5-6 min)
### Contexto, dataset, desafíos

### Diapositiva 1 — Portada
> "Buenos días. Vamos a presentar un sistema que mira imágenes de tejido de mama tomadas con microscopio y decide si lo que se ve es **benigno** (no cancerígeno) o **maligno** (cancerígeno). Lo construimos con una red neuronal y lo entrenamos sobre un dataset público llamado BreaKHis."

### Diapositiva 2 — Contexto clínico
> "El diagnóstico de cáncer de mama hoy funciona así: si un médico sospecha un tumor, se toma una muestra del tejido con una **biopsia**. Esa muestra se tiñe con dos colorantes (hematoxilina, que pinta los núcleos celulares de morado, y eosina, que pinta el resto rosado) y se mira al microscopio.
>
> El que mira es un patólogo: un médico especializado en mirar tejidos. Decide si las células están organizadas normalmente o si están creciendo de forma desordenada, lo cual es señal de cáncer.
>
> El problema es que esto toma tiempo, requiere un especialista entrenado, y dos patólogos pueden no estar de acuerdo. En sitios con pocos especialistas el diagnóstico se puede tardar semanas. Por eso hay interés en construir sistemas que automaticen al menos parte del proceso."

### Diapositiva 3 — El dataset BreaKHis
> "El dataset se llama BreaKHis. Lo creó un laboratorio de la Universidad Federal de Paraná en Brasil y lo publicaron en 2016. Contiene 9.109 imágenes de tejido de mama, de 82 pacientes, capturadas con microscopio a cuatro niveles de zoom: 40, 100, 200 y 400 aumentos.
>
> Hay dos clases:
> - Benigno: 2.480 imágenes. Son tumores que no se diseminan. Subtipos: adenosis, fibroadenoma, tumor filodes y adenoma tubular.
> - Maligno: 5.429 imágenes. Son cánceres. Subtipos: carcinoma ductal, lobular, mucinoso y papilar.
>
> Para nuestro experimento usamos un subconjunto: solo el fold 1 a 40 aumentos, que son 1.250 imágenes para entrenar y 745 para validar. La cifra exacta sale del archivo `Folds.csv` oficial, que ya viene con el dataset."

### Diapositiva 4 — Los tres desafíos
> "Trabajar con este dataset tiene tres desafíos que no aparecen en clasificación común de imágenes.
>
> **Primero, la variación de tinción.** Cada laboratorio prepara las muestras de forma ligeramente diferente y los colorantes nunca quedan idénticos. La misma clase de tumor puede verse más rosada en un hospital y más violeta en otro. Si entrenamos en un solo hospital, el modelo aprende ese tono específico y falla en otros.
>
> **Segundo, fugas de información entre paciente y modelo.** Cada paciente aportó varias imágenes al dataset. Si una imagen del paciente A está en entrenamiento y otra del mismo paciente A está en validación, el modelo no está aprendiendo a detectar cáncer: está aprendiendo a reconocer el tejido específico de ese paciente. Las métricas salen infladas y el modelo es inútil con pacientes nuevos. A esto se le llama **data leakage**. Por eso usamos los folds oficiales del paper: los autores garantizaron que ningún paciente aparece en train y test al mismo tiempo.
>
> **Tercero, desbalance de clases.** En el dataset hay más imágenes malignas que benignas (más o menos 70/30). Si simplemente entrenamos un modelo y le pedimos que maximice aciertos, va a aprender que predecir 'maligno' todo el tiempo le da 70% de precisión. Por eso medimos con métricas distintas, que Joan va a explicar."

---

## PARTE 2 — JOAN (7-8 min)
### Solución técnica y demo

### Diapositiva 5 — Qué construimos
> "Lo que armamos es una red neuronal convolucional que recibe una imagen y devuelve una probabilidad entre 0 y 1. Si la probabilidad es mayor a 0.5 lo clasifica como maligno, si es menor lo clasifica como benigno.
>
> La red que usamos se llama **ResNet50**. La gracia es que no la entrenamos desde cero: tomamos una versión que ya está pre-entrenada en ImageNet, un dataset de 1.2 millones de fotos de objetos comunes como perros, autos, sillas. Esa red ya sabe detectar bordes, texturas y formas. A esa técnica se le llama **Transfer Learning**: en vez de empezar de cero, partimos de algo que ya funciona y lo adaptamos a nuestro problema.
>
> Esto importa porque entrenar una red desde cero requiere millones de imágenes y semanas de cómputo. Nosotros tenemos 1.250 imágenes y una GPU gratuita en Colab."

### Diapositiva 6 — Cómo está hecha una red (cabeza y cuerpo)
> "Una red convolucional como ResNet50 tiene dos partes:
>
> - El **cuerpo** o **backbone**: son 50 capas que progresivamente convierten la imagen en un resumen numérico. Las primeras capas detectan cosas simples como bordes, las del medio detectan texturas, las últimas detectan patrones complejos. Al final del cuerpo, cada imagen se ha convertido en un vector de 2.048 números que describe sus características.
>
> - La **cabeza** o **head**: es la última capa que toma esos 2.048 números y decide la clase. La cabeza original de ResNet50 tenía 1.000 salidas, una por cada clase de ImageNet (perro, auto, etc). Nosotros la reemplazamos por una capa con una sola salida: la probabilidad de que el tejido sea maligno.
>
> Esto es lo que hace el código en la celda del modelo: carga el ResNet50 preentrenado, le quita la cabeza original, le pega una cabeza nueva con una sola neurona, y deja todo lo demás congelado."

### Diapositiva 7 — Las dos fases de entrenamiento
> "Entrenar el modelo se hace en dos fases.
>
> Antes de explicar las fases, dos palabras del vocabulario:
> - Una **época** es cuando el modelo ha visto todas las imágenes de entrenamiento exactamente una vez. Como tenemos 1.250 imágenes y procesamos de a 32 (lo que se llama un *batch*), una época son unos 40 batches.
> - Una **fase** es una etapa del entrenamiento con una estrategia distinta.
>
> **Fase 1 (Head-Only):** 5 épocas. Aquí solo entrenamos la cabeza, el cuerpo está congelado. La idea es: el cuerpo ya sabe extraer características de imágenes, solo necesitamos enseñarle a la cabeza a interpretar esas características en términos de 'benigno' o 'maligno'. Como solo entrenamos la cabeza (que son apenas 2.049 parámetros), esto es rapidísimo: unos 2 minutos.
>
> **Fase 2 (Fine-Tuning):** 5 épocas más. Aquí descongelamos el cuerpo y permitimos que también se ajuste, pero con un *learning rate* (velocidad de aprendizaje) muy chico: 1e-5 para el cuerpo y 1e-4 para la cabeza. La razón de tener velocidades distintas es: el cuerpo ya sabe mucho de visión general y no queremos que cambie demasiado. La cabeza es nueva y necesita más cambios. Esta fase tarda unos 4-5 minutos.
>
> En total: 10 épocas, unos 7 minutos de entrenamiento."

### Diapositiva 8 — Las métricas que usamos
> "Para evaluar si el modelo funciona miramos tres números:
>
> - **F1-Score Macro:** un promedio entre qué tan bien clasifica benignos y qué tan bien clasifica malignos. Lo queremos arriba de 0.90.
> - **Recall Maligno:** de todos los casos que eran realmente malignos, qué porcentaje detectó. Lo queremos arriba de 0.95. Esto es lo más importante clínicamente: si nos perdemos un maligno, el paciente no recibe tratamiento.
> - **AUC-ROC:** una medida general de qué tan bien separa las dos clases. Lo queremos arriba de 0.95.
>
> No miramos 'accuracy' (porcentaje total de aciertos) porque con clases desbalanceadas engaña: un modelo que predice 'maligno' siempre tendría 70% de accuracy sin haber aprendido nada."

### Demo en vivo — orden de las celdas
> "Vamos a Colab a ver el sistema corriendo."

(abrir `breakhis_colab.ipynb` en Colab)

**Orden sugerido para mostrar:**

1. **Celda de configuración:** "Aquí verificamos que tenemos GPU. Si dice 'cuda' es una Tesla T4 de Google."
2. **Celda de descarga:** "El dataset baja de Kaggle automáticamente con una línea. Pesa 3 GB."
3. **Celda de verificación de rutas:** "Esto es un assert que metimos para asegurarnos que los archivos existen antes de empezar a entrenar. Si algo está mal, falla acá y no diez minutos después."
4. **Celda de distribución de clases:** "Aquí se ve el desbalance: en train tenemos más malignos que benignos."
5. **Celda de muestras visuales:** "Estas son imágenes reales. La fila azul son benignos, la roja malignos. La diferencia es sutil, hay que tener ojo entrenado para verla."
6. **Celda del Dataset/DataLoader:** "Esta clase es un envoltorio sobre las imágenes. Le dice a PyTorch cómo leerlas y cómo aplicarles transformaciones (rotación aleatoria, flip horizontal, etc) para que el modelo no se memorice las imágenes exactas."
7. **Celda del modelo:** "Acá se ve lo que comenté antes: cargamos ResNet50, le quitamos la cabeza original y le ponemos una nueva. De los 25 millones de parámetros, solo 2.049 son entrenables al inicio."
8. **Celda de entrenamiento Fase 1:** "Cinco épocas, dos minutos."
9. **Celda de entrenamiento Fase 2:** "Cinco épocas más, cuatro minutos."
10. **Curvas de entrenamiento:** "Aquí se ve cómo la loss baja y el F1 sube. La línea vertical marca cuándo descongelamos el cuerpo."
11. **Matriz de confusión:** "Esto es lo importante clínicamente. Los **falsos negativos** son los casos donde el modelo dijo 'benigno' pero era 'maligno'. Es lo que más queremos minimizar."
12. **Predicciones individuales:** "Y acá ocho predicciones reales con la probabilidad que dio el modelo. Verde si acertó, rojo si falló."

---

## PARTE 3 — AMBOS (3-4 min)
### Conclusiones

### Diapositiva 9 — Resultados
**Joan dice:**
> "Después de 10 épocas en una GPU gratuita obtuvimos:"
>
> (insertar los números reales del Colab: F1-Macro, Recall Maligno, AUC)
>
> "Para comparar: el paper original de BreaKHis (2016) con modelos clásicos llegaba a unos 85% de accuracy. Con Transfer Learning sobre ResNet50 estamos por encima."

### Diapositiva 10 — Por qué priorizamos Recall sobre Precision
**Iván dice:**
> "Hay una decisión que vale la pena explicar. Elegimos maximizar **recall** de la clase maligna, no la precisión.
>
> En diagnóstico de screening, hay dos tipos de error:
> - Falso positivo: el modelo dice 'maligno' y era benigno. Consecuencia: el paciente hace estudios adicionales y se descarta. Molesto pero no fatal.
> - Falso negativo: el modelo dice 'benigno' y era maligno. Consecuencia: el paciente no recibe tratamiento. Potencialmente fatal.
>
> Por eso aceptamos algunos falsos positivos extra a cambio de minimizar los falsos negativos. Esta es una decisión clínica, no estética del modelo."

### Diapositiva 11 — Qué quedó afuera y qué haríamos con más tiempo
**Joan dice:**
> "Esta versión es simplificada. La versión completa que diseñamos en el documento técnico incluiría:
> - Un *ensemble*: dos redes (ResNet50 + DenseNet121) en paralelo, sumando sus predicciones. Aumenta robustez.
> - Entrenamiento en los 5 folds y promedio de resultados (validación cruzada real).
> - Normalización de tinción con OpenCV (CLAHE + Macenko) para reducir la variabilidad entre laboratorios.
> - Una API REST con FastAPI para exponer el modelo como un servicio web al que un médico le manda una imagen y recibe la predicción."

### Diapositiva 12 — Cierre
**Iván:**
> "En resumen: construimos un clasificador binario benigno/maligno sobre BreaKHis usando Transfer Learning, con split a nivel de paciente para evitar fugas de información. Las métricas finales son las que vieron en la matriz de confusión. ¿Preguntas?"

---

## GLOSARIO

### Términos médicos

| Término | Qué significa |
|---------|--------------|
| **Histopatología** | Estudio de tejidos al microscopio para diagnosticar enfermedades. Histo = tejido, patología = enfermedad. |
| **Biopsia** | Procedimiento donde se toma una pequeña muestra de tejido del paciente para analizarla. |
| **Tinción H&E** | Hematoxilina y Eosina. Dos colorantes que se aplican a los tejidos. La hematoxilina tiñe los núcleos celulares de violeta, la eosina tiñe el citoplasma y matriz de rosado. Es la tinción estándar mundial. |
| **Patólogo** | Médico especialista en mirar tejidos al microscopio y diagnosticar enfermedades a partir de ellos. |
| **Benigno** | Tumor que crece de forma controlada, no se disemina, generalmente no es fatal. |
| **Maligno** | Tumor canceroso. Puede invadir tejidos vecinos y formar metástasis. |
| **Adenosis** | Proliferación benigna de glándulas en el tejido mamario. |
| **Fibroadenoma** | Tumor benigno común en mujeres jóvenes. |
| **Tumor filodes** | Tumor mamario que puede ser benigno o maligno. En este dataset es el benigno. |
| **Adenoma tubular** | Tumor benigno con estructura tubular. |
| **Carcinoma ductal** | Cáncer que se origina en los conductos mamarios. El más común. |
| **Carcinoma lobular** | Cáncer que se origina en los lóbulos productores de leche. |
| **Carcinoma mucinoso** | Subtipo de cáncer con producción de mucina. |
| **Carcinoma papilar** | Subtipo de cáncer con forma de papila. |
| **Magnificación 40X, 100X, 200X, 400X** | Niveles de aumento del microscopio. A 40X se ve la arquitectura general del tejido, a 400X se ven detalles celulares. |
| **Screening** | Examen de tamizaje aplicado a una población para detectar enfermedad temprana. |

### Términos técnicos (Machine Learning / Deep Learning)

| Término | Qué significa |
|---------|--------------|
| **Red neuronal convolucional (CNN)** | Tipo de red especializada en imágenes. Aplica filtros (convoluciones) que detectan patrones visuales. |
| **ResNet50** | Una CNN específica con 50 capas. La 'Res' es por las 'conexiones residuales' que tiene, un truco que permite entrenar redes profundas sin que se rompa el entrenamiento. |
| **DenseNet121** | Otra CNN, con 121 capas, donde cada capa recibe entrada de todas las capas anteriores. |
| **ImageNet** | Dataset público con 1.2 millones de imágenes en 1.000 clases (perros, autos, etc). Es el dataset estándar para preentrenar redes. |
| **Transfer Learning** | Estrategia de entrenamiento donde se parte de un modelo ya entrenado en otra tarea y se adapta a la nueva tarea. Más eficiente que entrenar desde cero. |
| **Backbone / Cuerpo** | La parte mayoritaria de una red, encargada de extraer características. En nuestro caso son las primeras 49 capas de ResNet50. |
| **Head / Cabeza** | La última capa (o pocas capas) que toman las características extraídas por el backbone y producen la predicción final. |
| **Época (Epoch)** | Una pasada completa por todas las imágenes de entrenamiento. Si hay 1.250 imágenes en train, una época es procesar las 1.250. |
| **Batch** | Subconjunto de imágenes procesadas juntas. Usamos batch de 32, así una época son ~40 batches. |
| **Learning Rate (LR)** | Cuánto cambia el modelo en cada paso de entrenamiento. LR alto = aprende rápido pero puede oscilar. LR bajo = aprende lento pero estable. |
| **Loss / Pérdida** | Un número que mide qué tan mal está prediciendo el modelo. Entrenar = bajar la loss. |
| **BCEWithLogitsLoss** | Binary Cross Entropy con Logits. La función de loss que usamos para clasificación binaria. |
| **pos_weight** | Parámetro de BCEWithLogitsLoss que penaliza más los errores en una clase. Lo usamos para sesgar el modelo hacia detectar malignos. |
| **Logits** | Los números crudos que produce la última capa, antes de aplicar sigmoide. Pueden ser negativos o muy grandes. |
| **Sigmoide** | Función matemática que convierte cualquier número en una probabilidad entre 0 y 1. |
| **Optimizador (Adam)** | Algoritmo que actualiza los parámetros del modelo para reducir la loss. Adam es el estándar moderno. |
| **GradScaler / AMP** | Automatic Mixed Precision. Hacer los cálculos en float16 en vez de float32 para ahorrar memoria y acelerar entrenamiento en GPU. |
| **Data leakage / Fuga** | Cuando información del conjunto de validación contamina el entrenamiento. Hace que las métricas se vean mejor de lo que realmente son. |
| **Patient-level split** | Partir el dataset asegurando que todas las imágenes de un mismo paciente queden en el mismo conjunto. Evita data leakage. |
| **F1-Score** | Promedio armónico entre precisión y recall. Útil con clases desbalanceadas. |
| **F1 Macro** | F1 calculado independientemente para cada clase y promediado. Trata las clases por igual aunque tengan distinto tamaño. |
| **Recall (Sensibilidad)** | De los casos que eran realmente positivos, qué porcentaje detectó el modelo. |
| **Precision** | De los casos que el modelo dijo positivos, qué porcentaje eran realmente positivos. |
| **AUC-ROC** | Area Under the ROC Curve. Mide qué tan bien el modelo separa las dos clases en todos los umbrales posibles. 1.0 es perfecto, 0.5 es azar. |
| **Confusion Matrix** | Tabla 2x2 que muestra: TN, FP, FN, TP. La forma estándar de mirar errores de clasificación. |
| **Falso Negativo (FN)** | Era positivo, el modelo dijo negativo. En nuestro caso: era maligno, dijo benigno. El error más grave. |
| **Falso Positivo (FP)** | Era negativo, el modelo dijo positivo. Era benigno, dijo maligno. |
| **Early Stopping** | Parar el entrenamiento si la métrica de validación no mejora durante varias épocas seguidas. Evita overfitting. |
| **Overfitting** | Cuando el modelo aprende demasiado bien los datos de entrenamiento y deja de generalizar. Se nota porque la loss de train sigue bajando pero la de validación empieza a subir. |
| **Dropout** | Técnica de regularización que durante entrenamiento "apaga" al azar el 50% de las neuronas. Previene overfitting. |

---

## PREGUNTAS QUE PUEDE HACER EL PROFESOR (con respuestas honestas)

### Sobre código

**P: ¿Qué hace exactamente la línea `model.fc = nn.Sequential(...)`?**
> R: Reemplaza la última capa (la cabeza) de ResNet50. La cabeza original era una capa Linear de 2048 a 1000 (para las 1000 clases de ImageNet). La cambiamos por un Dropout más una capa Linear de 2048 a 1 (una sola salida, la probabilidad de ser maligno).

**P: ¿Por qué `squeeze(1)` después del forward?**
> R: La salida del modelo tiene forma [batch_size, 1]. Pero la loss BCEWithLogitsLoss espera que el target tenga la misma forma que el input, y nuestro target tiene forma [batch_size]. Con `squeeze(1)` aplastamos la dimensión 1, dejando ambos en [batch_size].

**P: ¿Qué hace `torch.autocast`?**
> R: Activa Mixed Precision Training. Le dice a PyTorch que use float16 en operaciones donde no haga falta float32. Reduce a la mitad el uso de memoria y acelera el entrenamiento en GPU. Para que sea numéricamente estable se combina con `GradScaler`, que escala los gradientes para evitar underflow.

**P: ¿Qué pasa si no congelan el backbone en Fase 1?**
> R: Como los pesos del cuerpo ya saben de visión general pero nuestra cabeza arranca con pesos aleatorios, los gradientes iniciales serían enormes y desordenados. Esos gradientes se propagan hacia atrás y destruyen lo que el backbone ya sabía. Por eso primero entrenamos solo la cabeza con backbone congelado, y luego en Fase 2 con todo descongelado pero learning rate muy bajo.

**P: ¿Por qué `pos_weight = n_malign / n_benign` y no al revés?**
> R: Honesta: la fórmula estándar para compensar desbalance sería al revés. Lo que estamos haciendo es una decisión clínica: queremos que el modelo prefiera predecir 'maligno' aunque haya algún falso positivo, porque clínicamente es más grave perderse un cáncer. Con pos_weight > 1, los errores en malignos penalizan más, así que el modelo aprende a "ser pesimista" y detectar todos los posibles cánceres.

**P: ¿Qué hace `RandomHorizontalFlip` y por qué se aplica?**
> R: En cada época, con probabilidad 50%, voltea la imagen horizontalmente. Es data augmentation: aumentamos artificialmente la variedad del dataset sin recolectar imágenes nuevas. En histopatología tiene sentido porque la orientación de la imagen no importa para el diagnóstico.

**P: ¿Por qué la imagen se redimensiona a 224x224?**
> R: Es el tamaño estándar de entrada de ResNet50 y muchas redes preentrenadas en ImageNet. Las imágenes originales son 700x460. Las redimensionamos a 224x224 para usar la red preentrenada tal cual. El paper original sugiere 448x448 pero eso requiere más memoria GPU.

**P: ¿Cuánto demora cada parte?**
> R: Descarga del dataset ~2 min, Fase 1 ~2 min, Fase 2 ~4-5 min. Total ~10 min en una Tesla T4 gratuita de Colab.

### Sobre arquitectura

**P: ¿Por qué ResNet50 y no algo más nuevo?**
> R: ResNet50 es el baseline estándar en la literatura de imágenes médicas, lo cual nos permite comparar con papers. Modelos más nuevos como Vision Transformers (ViT) requieren datasets más grandes para superar a las CNNs. Para 1.250 imágenes de entrenamiento, ResNet50 con Transfer Learning es la mejor opción práctica.

**P: ¿Qué son las conexiones residuales de ResNet?**
> R: Son "atajos" que conectan la salida de una capa con la entrada de una capa varios pasos después, sumando ambas. Permiten entrenar redes muy profundas sin que el gradiente se desvanezca o explote. Sin estas conexiones, redes de más de 20 capas eran muy difíciles de entrenar.

### Sobre dataset

**P: ¿Por qué solo usaron un fold y una magnificación?**
> R: Por tiempo. El diseño completo entrenaría sobre los 5 folds y las 4 magnificaciones. Cada combinación toma ~10 min, así que la validación cruzada completa sería ~3-5 horas. Para la demo elegimos fold 1 a 40X, que es la combinación más representativa.

**P: ¿Cómo sé que el modelo no está haciendo trampa con data leakage?**
> R: Porque usamos los folds oficiales que vienen en el archivo `Folds.csv`. Los autores de BreaKHis ya hicieron el split a nivel de paciente: ningún paciente aparece en train y test simultáneamente. Eso lo verificamos al filtrar el DataFrame.

**P: ¿Qué pasa si en la práctica recibo una imagen de un microscopio distinto?**
> R: Probablemente el modelo va a fallar más, porque está entrenado en imágenes de un solo laboratorio. Para producción real haría falta normalización de tinción (CLAHE + Macenko) y entrenamiento sobre múltiples centros.

### Sobre métricas

**P: ¿Por qué Recall es más importante que Precision?**
> R: En screening médico, perder un caso positivo (falso negativo) es más grave que dar una alarma falsa (falso positivo). Un falso positivo manda al paciente a hacer un estudio extra, un falso negativo lo manda a casa con cáncer no detectado. Maximizar recall = minimizar falsos negativos.

**P: ¿Cuál es la relación entre Recall y pos_weight?**
> R: pos_weight en la loss empuja al modelo a ser más sensible a los positivos durante el entrenamiento. Eso resulta en que en inferencia, predice 'maligno' más fácilmente. Eso sube el recall (detecta más malignos) a costa de bajar la precision (algunos benignos los marca como malignos).

**P: ¿Qué significa AUC-ROC de 0.95?**
> R: Que si tomo un caso maligno al azar y uno benigno al azar, el modelo le va a asignar mayor probabilidad de malignidad al maligno el 95% de las veces. Es una medida que no depende del umbral de decisión (que en nuestro código es 0.5).

### Si no sabemos la respuesta

> "Honestamente, no estoy seguro/a de la respuesta exacta a esa pregunta, pero según el paper de BreaKHis [o lo que sea aplicable]... lo que me queda claro es [hasta donde sí entendemos]."

Es mejor admitir que no sabemos algo que inventar.

---

## CHECKLIST DEL DÍA

### El día antes
- [ ] Ejecutar el notebook completo en Colab
- [ ] Descargar las 5 imágenes PNG generadas y el .pth del modelo
- [ ] Anotar las métricas finales reales para citarlas en el cierre
- [ ] Repasar el glosario una vez para no trabarse en términos

### El día
- [ ] Llegar 15 min antes, abrir Colab
- [ ] Verificar GPU activa y re-ejecutar las celdas hasta dejar el modelo entrenado (~7 min)
- [ ] Tener las PNGs y el guión abiertos en otra pestaña por si Colab se cae
