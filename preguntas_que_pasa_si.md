# Preguntas tipo "¿Qué pasa si...?" del profesor
### Respuestas honestas, sin inventar

Estas son las preguntas más probables que un profesor hace en este tipo de presentación. Las respuestas están en lenguaje normal, no técnico, para que cualquiera de los dos las pueda responder.

---

## Sobre los datos

### ¿Qué pasa si entrenas con otro fold?
Los resultados serían parecidos pero no idénticos. Cada fold tiene pacientes distintos y el modelo aprende cosas un poco diferentes. Por eso el paper original recomienda promediar los resultados de los 5 folds.

### ¿Qué pasa si usas una magnificación distinta, por ejemplo 400X en vez de 40X?
A 400X se ven detalles celulares más finos. El modelo puede ser más preciso en algunos tumores y menos en otros. Los autores del paper reportan que combinar las 4 magnificaciones da los mejores resultados.

### ¿Qué pasa si una imagen viene de otro hospital con tinción distinta?
El modelo probablemente va a fallar más. Está entrenado sobre imágenes de un solo laboratorio. Para producción real haría falta normalización de tinción y entrenar con datos de varios centros.

### ¿Qué pasa si quitas las rotaciones aleatorias del entrenamiento?
El modelo aprende a depender de la orientación de las imágenes. Funciona peor con imágenes en orientaciones que no vio. Para histopatología eso es malo: una muestra de tejido no tiene "arriba" o "abajo" intrínseco.

### ¿Qué pasa si el dataset estuviera balanceado 50/50?
No necesitaríamos `pos_weight`. La métrica accuracy sería confiable. Pero también el modelo tendría menos información sobre la clase maligna (la que más nos importa).

### ¿Qué pasa si tienes data leakage entre train y val?
Las métricas de validación salen infladas. El modelo se ve perfecto pero en producción falla. Por eso usamos los folds oficiales del paper, que garantizan separación por paciente.

---

## Sobre el modelo

### ¿Qué pasa si entrenas más épocas? ¿Por ejemplo 50?
Al principio sigue mejorando. Después de cierto punto el modelo empieza a memorizar el train set (overfitting): la loss de train sigue bajando pero la de validación sube. Por eso pusimos early stopping de 4 épocas.

### ¿Qué pasa si no congelas el backbone en la Fase 1?
Los pesos del backbone son muy buenos (entrenados sobre 1.2M de imágenes). Si los entrenas desde el principio con una cabeza nueva al azar, los gradientes desordenados los destruyen. El modelo arranca peor que antes y tarda muchas épocas en recuperarse.

### ¿Qué pasa si usas un learning rate más alto en la Fase 2?
El backbone se "rompe": olvida lo que aprendió de ImageNet. Por eso usamos 1e-5, que es muy pequeño. Es un ajuste finito, no un re-entrenamiento.

### ¿Qué pasa si reemplazas ResNet50 por ResNet18?
Es más chica (11M params vs 25M). Entrena más rápido y consume menos memoria. Probablemente da un F1 un poco menor (~1-2%) pero sigue siendo competitiva. Para datasets chicos a veces es mejor que ResNet50 porque tiene menos riesgo de overfitting.

### ¿Qué pasa si usas un modelo más nuevo como un Vision Transformer (ViT)?
Los ViT son más nuevos y muy poderosos, pero requieren más datos para superar a las CNNs. Con 1.250 imágenes de entrenamiento, ResNet50 con Transfer Learning sigue siendo competitivo o mejor que ViT.

### ¿Qué pasa si quitas el Dropout?
El modelo puede sobreajustarse más fácilmente. Dropout apaga al azar el 50% de las neuronas durante entrenamiento, forzando al modelo a no depender de una sola conexión. Sin él, las métricas de validación pueden empeorar.

### ¿Qué pasa si entrenas desde cero, sin Transfer Learning?
Necesitarías muchísimos más datos y tiempo. Con 1.250 imágenes, una ResNet50 desde cero probablemente da F1 < 0.50. El Transfer Learning es lo que hace este proyecto factible.

---

## Sobre las métricas y el umbral de decisión

### ¿Qué pasa si bajas el umbral de decisión de 0.5 a 0.3?
El modelo predice "maligno" más fácilmente. Sube el Recall Maligno (detecta más cánceres) pero también sube la cantidad de falsos positivos (alarmas falsas). En screening clínico esto puede ser deseable.

### ¿Qué pasa si subes el umbral a 0.7?
El modelo es más conservador con el diagnóstico de maligno. Baja el Recall (se pierde más cánceres) pero sube la Precision (los que detecta son más probables de ser realmente malignos). Para screening esto es contraproducente.

### ¿Qué pasa si la métrica accuracy es 90% pero el Recall Maligno es 60%?
Significa que el modelo predice "benigno" casi siempre, acierta los benignos pero se pierde el 40% de los malignos. Clínicamente es un desastre. Por eso no usamos accuracy como métrica principal.

### ¿Qué pasa si reportas solo F1 y no Recall?
F1 promedia precision y recall. Un modelo con F1=0.85 puede tener Recall=0.95 o Recall=0.75. La diferencia es importante clínicamente. Hay que reportar las dos.

### ¿Qué pasa si dos clases están balanceadas? ¿Necesitas F1 Macro o sirve F1 normal?
Con clases balanceadas, F1 Macro y F1 promedio dan casi lo mismo. Con clases desbalanceadas (como en BreaKHis), F1 Macro es más justo porque no le da más peso a la clase mayoritaria.

---

## Sobre el código y el entorno

### ¿Qué pasa si Colab no te da una GPU?
Todo el código corre, pero el entrenamiento pasa de 7 minutos a 1-2 horas. El código está preparado para detectarlo y usar CPU.

### ¿Qué pasa si te quedas sin memoria GPU?
Hay que bajar el `BATCH_SIZE` de 32 a 16. Funciona pero entrena un poco más lento. La calidad final del modelo no cambia.

### ¿Qué pasa si dos personas corren el mismo notebook con el mismo seed?
Obtienen casi los mismos resultados. "Casi" porque algunas operaciones en GPU no son 100% deterministas. Pero las métricas van a estar dentro de un 1%.

### ¿Qué pasa si pierdes la conexión a Colab a la mitad del entrenamiento?
Si guardaste el modelo en Drive (no en el cache de Colab), lo podés cargar y seguir. Si no, hay que volver a empezar. Por eso el `torch.save()` al final es importante.

### ¿Qué pasa si cambias el orden de las celdas?
Algunas variables se usan antes de definirse → `NameError`. Para evitar esto, usá "Ejecutar todas" desde el menú.

---

## Sobre lo clínico (limitaciones honestas)

### ¿Esto se puede usar en un hospital de verdad?
No directamente. Faltaría:
- Certificación regulatoria (FDA, CE)
- Validación con varios centros
- Estudios clínicos prospectivos
- Integración con el sistema de imágenes del hospital

Sí se usa en investigación clínica para asistir al patólogo, nunca para reemplazarlo.

### ¿El modelo va a reemplazar al patólogo?
No. Esta tipo de herramientas se diseñan como apoyo: el patólogo sigue tomando la decisión final, el modelo le ayuda a priorizar casos sospechosos o le da una segunda opinión.

### ¿Qué pasa si el modelo dice "benigno" pero el paciente tiene cáncer?
Es un falso negativo. Por eso priorizamos el Recall y por eso ningún modelo se debería usar solo. El pipeline real siempre incluye revisión humana.

### ¿Por qué solo binario y no clasificar el tipo específico de cáncer?
Por simplicidad y por tamaño de muestra. Distinguir entre 8 subtipos requiere más datos por clase. El paper original también hace ambas tareas y reporta que la binaria es más confiable.

---

## Si no sabes la respuesta

> "Honestamente no me sé el detalle exacto. Lo que sí sé es [explica lo más cercano que sepas]. Si querés que averigüe la respuesta exacta, te la mando después."

Es 100% válido. Es mejor que inventar.
