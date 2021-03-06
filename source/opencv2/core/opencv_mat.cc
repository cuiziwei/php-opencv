#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../../../php_opencv.h"
#include "opencv_mat.h"
#include "opencv_type.h"
#include "../../../opencv_exception.h"

zend_object_handlers opencv_mat_object_handlers;

zend_class_entry *opencv_mat_ce;

/**
 * @param type
 * @return
 */
zend_object* opencv_mat_create_handler(zend_class_entry *type)
{
    int size = sizeof(opencv_mat_object);
    opencv_mat_object *obj = (opencv_mat_object *)ecalloc(1,size);
    memset(obj, 0, sizeof(opencv_mat_object));
    zend_object_std_init(&obj->std, type);
    object_properties_init(&obj->std, type);
    obj->std.ce = type;
    obj->std.handlers = &opencv_mat_object_handlers;
    return &obj->std;
}


void opencv_mat_free_obj(zend_object *object)
{
    opencv_mat_object *obj;
    obj = get_mat_obj(object);
    delete obj->mat;
    zend_object_std_dtor(object);
}

void opencv_mat_update_property_by_c_mat(zval *z,Mat *mat){
    zend_update_property_long(opencv_mat_ce, z, "rows", sizeof("rows")-1, mat->rows);
    zend_update_property_long(opencv_mat_ce, z, "cols", sizeof("cols")-1, mat->cols);
    zend_update_property_long(opencv_mat_ce, z, "type", sizeof("type")-1, mat->type());
}

/**
 * Mat __construct
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, __construct)
{
    long rows, cols, type;
    zval *scalar_zval = NULL;
    Scalar scalar;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lll|O", &rows, &cols, &type, &scalar_zval,opencv_scalar_ce) == FAILURE) {
        RETURN_NULL();
    }

    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    if(scalar_zval != NULL){
        opencv_scalar_object *scalar_object = Z_PHP_SCALAR_OBJ_P(scalar_zval);
        scalar = *(scalar_object->scalar);
    }else{
        scalar=Scalar(0);
    }
    Mat M((int)rows, (int)cols, (int)type);
//    obj->mat = new Mat(M);
    obj->mat = new Mat((int)rows, (int)cols, (int)type, scalar);
    //obj->mat = new Mat((int)rows, (int)cols, (int)type); //TODO Why Mat array not correct
    opencv_mat_update_property_by_c_mat(getThis(), obj->mat);
}

/**
 * print Mat data
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, print)
{
    long type = Formatter::FMT_DEFAULT;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &type) == FAILURE) {
        RETURN_NULL();
    }
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    std::cout << format(*(obj->mat),int(type)) << std::endl;
    RETURN_NULL();
}


PHP_METHOD(opencv_mat, type)
{
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    RETURN_LONG(obj->mat->type());
}

PHP_METHOD(opencv_mat, depth)
{
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    RETURN_LONG(obj->mat->depth());
}

PHP_METHOD(opencv_mat, channels)
{
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    RETURN_LONG(obj->mat->channels());
}


PHP_METHOD(opencv_mat, zeros)
{
    long rows, cols, flags = IMREAD_COLOR;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll|l", &rows, &cols, &flags) == FAILURE) {
        RETURN_NULL();
    }
    zval instance;
    object_init_ex(&instance, opencv_mat_ce);
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(&instance);

    Mat im = Mat::zeros((int)rows, (int)cols, (int)flags);

    obj->mat=new Mat(im);
    //update php Mat object property
    opencv_mat_update_property_by_c_mat(&instance, obj->mat);

    RETURN_ZVAL(&instance,0,0); //return php Mat object
}


/**
 * Mat->clone()
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, clone)
{
    zval instance;
    object_init_ex(&instance, opencv_mat_ce);
    opencv_mat_object *new_obj = Z_PHP_MAT_OBJ_P(&instance);
    opencv_mat_object *old_obj = Z_PHP_MAT_OBJ_P(getThis());
    Mat new_mat = old_obj->mat->clone();

    new_obj->mat = new Mat(new_mat);
    //update php Mat object property
    opencv_mat_update_property_by_c_mat(&instance, new_obj->mat);

    RETURN_ZVAL(&instance,0,0); //return php Mat object
}

/**
 * Mat->isContinuous
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, is_continuous)
{
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    bool isContinuous = obj->mat->isContinuous();
    RETURN_BOOL(isContinuous);
}

/**
 * Mat->row(y)
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, row)
{
    long y;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &y) == FAILURE) {
        RETURN_NULL();
    }
    zval instance;
    object_init_ex(&instance, opencv_mat_ce);
    opencv_mat_object *new_obj = Z_PHP_MAT_OBJ_P(&instance);

    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    Mat im = obj->mat->row(y);
    new_obj->mat=new Mat(im);
    opencv_mat_update_property_by_c_mat(&instance, new_obj->mat);

    RETURN_ZVAL(&instance,0,0); //return php Mat object
}

/**
 * Mat->col(x)
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, col)
{
    long x;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &x) == FAILURE) {
        RETURN_NULL();
    }
    zval instance;
    object_init_ex(&instance, opencv_mat_ce);
    opencv_mat_object *new_obj = Z_PHP_MAT_OBJ_P(&instance);

    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    Mat im = obj->mat->col(x);
    new_obj->mat=new Mat(im);

    opencv_mat_update_property_by_c_mat(&instance, new_obj->mat);

    RETURN_ZVAL(&instance,0,0); //return php Mat object
}

/**
 * Mat->getImageROI(x)
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, get_image_roi)
{
    zval *rect_zval;
    zval instance;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &rect_zval,opencv_rect_ce) == FAILURE) {
        RETURN_NULL();
    }
    opencv_rect_object *rect_object = Z_PHP_RECT_OBJ_P(rect_zval);

    object_init_ex(&instance, opencv_mat_ce);
    opencv_mat_object *new_obj = Z_PHP_MAT_OBJ_P(&instance);

    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(getThis());
    try {
        Mat roi = (*obj->mat)(*rect_object->rect);
        new_obj->mat=new Mat(roi);
    }catch (Exception exception){
        const char* err_msg = exception.what();
        opencv_throw_exception(err_msg);//throw exception
        RETURN_NULL();
    }

    opencv_mat_update_property_by_c_mat(&instance, new_obj->mat);

    RETURN_ZVAL(&instance,0,0); //return php Mat object
}

ZEND_BEGIN_ARG_INFO_EX(opencv_mat_copy_to_arginfo, 0, 0, 2)
                ZEND_ARG_INFO(1, m)
                ZEND_ARG_INFO(0, mask)
ZEND_END_ARG_INFO()

/**
 * Mat->copyTo(Mat $mat,Mat $mask=NULL)
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, copy_to)
{
    zval *m_zval, *mask_zval = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|O", &m_zval, opencv_mat_ce, &mask_zval, opencv_mat_ce) == FAILURE) {
        RETURN_NULL();
    }

    zval *m_real_zval = Z_REFVAL_P(m_zval);
    opencv_mat_object *m_object;
    if(Z_TYPE_P(m_real_zval) == IS_OBJECT && Z_OBJCE_P(m_real_zval)==opencv_mat_ce){
        m_object = Z_PHP_MAT_OBJ_P(m_real_zval);
    } else if(Z_TYPE_P(m_real_zval) == IS_NULL){
        zval instance;
        Mat dst;
        object_init_ex(&instance,opencv_mat_ce);
        ZVAL_COPY_VALUE(m_real_zval, &instance);
        m_object = Z_PHP_MAT_OBJ_P(m_real_zval);
        m_object->mat = new Mat(dst);
    }else{
        char *error_message = (char*)malloc(strlen("copy expect param is Mat object or null.")+ 1);
        strcpy(error_message,"copy expect param is Mat object or null.");
        opencv_throw_exception(error_message);
        free(error_message);
    }
    opencv_mat_object *this_object = Z_PHP_MAT_OBJ_P(getThis());
    try {
        if(mask_zval != NULL){
            opencv_mat_object *mask_object = Z_PHP_MAT_OBJ_P(mask_zval);
            this_object->mat->copyTo(*m_object->mat, *mask_object->mat);
        }else{
            this_object->mat->copyTo(*m_object->mat);
        }
        opencv_mat_update_property_by_c_mat(m_real_zval, m_object->mat);
    }catch (Exception exception){
        const char* err_msg = exception.what();
        opencv_throw_exception(err_msg);//throw exception
    }


    RETURN_NULL();

}

/**
 * CV\Mat->at
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, at)
{
    long row, col, channel;
    zval *value_zval = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "lll|z", &row, &col, &channel, &value_zval) == FAILURE) {
        RETURN_NULL();
    }
    opencv_mat_object *this_object = Z_PHP_MAT_OBJ_P(getThis());
    if(value_zval == NULL){
        //get px value
        RETURN_LONG(this_object->mat->at<Vec3b>((int)row,(int)col)[channel]/1);
    }else{
        //set px value
        convert_to_long(value_zval);
        zend_long value = Z_LVAL_P(value_zval);
        this_object->mat->at<Vec3b>((int)row,(int)col)[channel]=saturate_cast<uchar>(value);
    }
    RETURN_NULL();
}


ZEND_BEGIN_ARG_INFO_EX(opencv_mat_convert_to_arginfo, 0, 0, 4)
                ZEND_ARG_INFO(1, dst)
                ZEND_ARG_INFO(0, rtype)
                ZEND_ARG_INFO(0, alpha)
                ZEND_ARG_INFO(0, beta)
ZEND_END_ARG_INFO()

/**
 * CV\Mat->convertTo
 * @param execute_data
 * @param return_value
 */
PHP_METHOD(opencv_mat, convert_to){
    zval *dst_zval;
    long rtype;
    double alpha = 1, beta = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl|dd", &dst_zval, &rtype, &alpha, &beta) == FAILURE) {
        RETURN_NULL();
    }
    zval *dst_real_zval = Z_REFVAL_P(dst_zval);
    opencv_mat_object *dst_object, *this_object;
    if(Z_TYPE_P(dst_real_zval) == IS_OBJECT && Z_OBJCE_P(dst_real_zval)==opencv_mat_ce){
        // is Mat object
        dst_object = Z_PHP_MAT_OBJ_P(dst_real_zval);
    } else{
        // isn't Mat object
        zval instance;
        Mat dst;
        object_init_ex(&instance,opencv_mat_ce);
        ZVAL_COPY_VALUE(dst_real_zval, &instance);// Cover dst_real_zval by Mat object
        dst_object = Z_PHP_MAT_OBJ_P(dst_real_zval);
        dst_object->mat = new Mat(dst);
    }
    this_object = Z_PHP_MAT_OBJ_P(getThis());
    this_object->mat->convertTo(*dst_object->mat, (int)rtype, alpha, beta);
    opencv_mat_update_property_by_c_mat(dst_real_zval,dst_object->mat);
    RETURN_NULL();
}


/**
 * opencv_mat_methods[]
 */
const zend_function_entry opencv_mat_methods[] = {
        PHP_ME(opencv_mat, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
        PHP_ME(opencv_mat, type, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(opencv_mat, depth, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(opencv_mat, channels, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(opencv_mat, print, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(opencv_mat, zeros, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
        PHP_MALIAS(opencv_mat, isContinuous ,is_continuous, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(opencv_mat, row, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(opencv_mat, col, NULL, ZEND_ACC_PUBLIC)
        PHP_ME(opencv_mat, at, NULL, ZEND_ACC_PUBLIC)
        PHP_MALIAS(opencv_mat, getImageROI ,get_image_roi, NULL, ZEND_ACC_PUBLIC)
        PHP_MALIAS(opencv_mat, copyTo ,copy_to, opencv_mat_copy_to_arginfo, ZEND_ACC_PUBLIC)
        PHP_MALIAS(opencv_mat, convertTo ,convert_to, opencv_mat_convert_to_arginfo, ZEND_ACC_PUBLIC)
        PHP_FE_END
};
/* }}} */



/**
 * Mat Class write_property
 * @param object
 * @param member
 * @param value
 * @param cache_slot
 */
void opencv_mat_write_property(zval *object, zval *member, zval *value, void **cache_slot){

    zend_string *str = zval_get_string(member);
    char *memberName=ZSTR_VAL(str);
    opencv_mat_object *obj = Z_PHP_MAT_OBJ_P(object);

    if(strcmp(memberName, "cols") == 0 && obj->mat->cols!=(int)zval_get_long(value)){
        obj->mat->cols=(int)zval_get_long(value);
    }else if(strcmp(memberName, "rows") == 0 && obj->mat->rows!=(int)zval_get_long(value)){
        obj->mat->rows=(int)zval_get_long(value);
    }
    zend_string_release(str);//free zend_string not memberName(zend_string->val)
    std_object_handlers.write_property(object,member,value,cache_slot);

}

/**
 * Mat Init
 */
void opencv_mat_init(void){
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce,OPENCV_NS, "Mat", opencv_mat_methods);
    opencv_mat_ce = zend_register_internal_class(&ce);

    opencv_mat_ce->create_object = opencv_mat_create_handler;
    memcpy(&opencv_mat_object_handlers,
           zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    opencv_mat_object_handlers.clone_obj = NULL;
    opencv_mat_object_handlers.write_property = opencv_mat_write_property;

    zend_declare_property_null(opencv_mat_ce,"type",sizeof("type") - 1,ZEND_ACC_PRIVATE);//private Mat->type
    opencv_mat_object_handlers.free_obj = opencv_mat_free_obj;
}






